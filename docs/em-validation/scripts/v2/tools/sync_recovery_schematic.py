"""Synchronize the existing V2 label-connected hierarchy without regenerating it.

Default: inspect and report only. Apply with --write; --root selects an isolated
copy for review. Existing root/sheet/component/pin UUIDs and coordinates survive.
Raw balanced S-expression edits preserve objects unknown to this helper, including
global labels. This deliberately refuses wired or nested hierarchies rather than
guessing how label changes interact with wires. KiCad netlist export is a separate
required integration check; see the isolated self-test in the recovery review.
"""
from __future__ import annotations

import argparse
from dataclasses import dataclass
from datetime import datetime, timezone
import json
import math
import os
from pathlib import Path
import re
import shutil
import uuid

import design_model


TOKEN = re.compile(r'"(?:\\.|[^"\\])*"|[()]|[^\s()]+', re.DOTALL)


@dataclass
class Atom:
    start: int
    end: int
    value: str


@dataclass
class Node:
    start: int
    end: int
    children: list

    @property
    def head(self):
        return self.children[0].value if self.children and isinstance(self.children[0], Atom) else ""

    def nodes(self, name):
        return [c for c in self.children if isinstance(c, Node) and c.head == name]

    def one(self, name, optional=False):
        found = self.nodes(name)
        if optional and not found:
            return None
        if len(found) != 1:
            raise ValueError(f"Expected one {name} in {self.head}, found {len(found)}")
        return found[0]

    def atom(self, index=1):
        result = self.children[index]
        if not isinstance(result, Atom):
            raise ValueError(f"Expected atom {index} in {self.head}")
        return result


def parse(text):
    stack, roots = [], []
    for match in TOKEN.finditer(text):
        token = match.group()
        if token == "(":
            node = Node(match.start(), -1, [])
            (stack[-1].children if stack else roots).append(node)
            stack.append(node)
        elif token == ")":
            if not stack:
                raise ValueError("Unbalanced closing parenthesis")
            stack.pop().end = match.end()
        else:
            value = json.loads(token) if token.startswith('"') else token
            (stack[-1].children if stack else roots).append(Atom(match.start(), match.end(), value))
    if stack or len(roots) != 1 or not isinstance(roots[0], Node):
        raise ValueError("Expected exactly one balanced root S-expression")
    return roots[0]


def quote(value):
    return json.dumps(str(value), ensure_ascii=False)


def raw(text, node):
    return text[node.start:node.end]


def edits(text, changes):
    result, edge = text, len(text)
    for start, end, replacement in sorted(changes, reverse=True):
        if not 0 <= start <= end <= edge:
            raise ValueError("Overlapping or invalid edits")
        result = result[:start] + replacement + result[end:]
        edge = start
    return result


def prop(node, name, optional=False):
    found = [p for p in node.nodes("property") if p.atom().value == name]
    if not found and optional:
        return None
    if len(found) != 1:
        raise ValueError(f"Expected one property {name}")
    return found[0]


def property_value(node, name):
    return prop(node, name).atom(2).value


def update_properties(text, values, *, dnp=None):
    node, changes, additions = parse(text), [], []
    for name, value in values.items():
        field = prop(node, name, optional=True)
        if field:
            atom = field.atom(2)
            if atom.value != str(value):
                changes.append((atom.start, atom.end, quote(value)))
        else:
            at = node.one("at", optional=True)
            x, y = (at.atom().value, at.atom(2).value) if at else ("0", "0")
            additions.append(f'(property {quote(name)} {quote(value)} (at {x} {y} 0) '
                             '(effects (font (size 1.27 1.27)) (hide yes)))')
    if dnp is not None:
        field = node.one("dnp", optional=True)
        value = "yes" if dnp else "no"
        if field:
            atom = field.atom()
            if atom.value != value:
                changes.append((atom.start, atom.end, value))
        else:
            additions.append(f"(dnp {value})")
    if additions:
        changes.append((node.end-1, node.end-1, "\n\t" + "\n\t".join(additions) + "\n"))
    return edits(text, changes)


def desired_lib(item):
    return item.symbol if item.symbol and ":" in item.symbol else "CodeSDR:" + item.symbol_name


def metadata(item):
    return dict(Value=item.value, Footprint=item.footprint,
                Datasheet=item.datasheet, Description=item.description)


def symbol_pins(symbol_text, unit=1):
    result = {}
    symbol = parse(symbol_text)
    if symbol.one("extends", optional=True):
        raise ValueError("Inherited library symbol must be flattened before use")
    for child in symbol.nodes("symbol"):
        match = re.search(r"_(\d+)_(\d+)$", child.atom().value)
        if match and (int(match[1]) not in (0, unit) or int(match[2]) not in (0, 1)):
            continue
        for pin in child.nodes("pin"):
            number = pin.one("number").atom().value
            at = pin.one("at")
            point = (float(at.atom().value), float(at.atom(2).value))
            if number in result and result[number] != point:
                raise ValueError(f"Multiple locations for pin {number}")
            result[number] = point
    if not result:
        raise ValueError(f"No pins in {symbol.atom().value}")
    return result


def pin_positions(instance_text, symbol_text):
    instance = parse(instance_text)
    if instance.one("mirror", optional=True):
        raise ValueError("Mirrored schematic symbols require an explicitly reviewed transform")
    at = instance.one("at")
    x, y, angle = (float(at.atom(i).value) for i in (1, 2, 3))
    theta = math.radians(angle)
    cosine, sine = math.cos(theta), math.sin(theta)
    pins = symbol_pins(symbol_text, int(instance.one("unit").atom().value))
    return {number: (round(x+px*cosine-py*sine, 4), round(y-px*sine-py*cosine, 4))
            for number, (px, py) in pins.items()}


def format_custom_instance(instance_text, symbol_text):
    """Place visible fields above a generated custom symbol's body."""
    node = parse(instance_text)
    x, y, angle = (float(node.one('at').atom(i).value) for i in (1, 2, 3))
    cosine, sine = math.cos(math.radians(angle)), math.sin(math.radians(angle))
    points = []
    for child in parse(symbol_text).nodes('symbol'):
        for rectangle in child.nodes('rectangle'):
            start, end = rectangle.one('start'), rectangle.one('end')
            for px in (float(start.atom().value), float(end.atom().value)):
                for py in (float(start.atom(2).value), float(end.atom(2).value)):
                    points.append((x+px*cosine-py*sine, y-px*sine-py*cosine))
    if not points:
        return instance_text
    left, top = min(p[0] for p in points), min(p[1] for p in points)
    changes = []
    for name, offset in [('Reference', 5.08), ('Value', 2.54)]:
        field = prop(node, name)
        at, effects = field.one('at'), field.one('effects')
        changes.append((at.start, at.end, f'(at {left:.4f} {top-offset:.4f} 0)'))
        justify = effects.one('justify', optional=True)
        if justify:
            changes.append((justify.start, justify.end, '(justify left)'))
        else:
            changes.append((effects.end-1, effects.end-1, ' (justify left)'))
    return edits(instance_text, changes)


def custom_label_angles(instance_text, symbol_text):
    """Orient labels outside the horizontal pin columns used by our generator."""
    node = parse(instance_text)
    if float(node.one('at').atom(3).value) % 180:
        return {}  # Preserve reviewed label formatting on rotated instances.
    cx, _ = at_point(node)
    return {point: 180 if point[0] < cx else 0
            for point in pin_positions(instance_text, symbol_text).values() if point[0] != cx}


def orient_global_label(label_text, angle):
    """Change only text direction/alignment, preserving its electrical anchor."""
    node = parse(label_text)
    if node.head != 'global_label' or angle not in (0, 180):
        raise ValueError('Expected a horizontal global label')
    at, effects = node.one('at'), node.one('effects')
    changes = [(at.atom(3).start, at.atom(3).end, str(angle))]
    justify = effects.one('justify', optional=True)
    align = 'right' if angle == 180 else 'left'
    if justify:
        changes.append((justify.start, justify.end, f'(justify {align})'))
    else:
        changes.append((effects.end-1, effects.end-1, f' (justify {align})'))
    return edits(label_text, changes)


def renamed_symbol(symbol_text, target, item):
    node = parse(symbol_text)
    old_name = node.atom().value.split(":")[-1]
    new_name = target.split(":")[-1]
    changes = [(node.atom().start, node.atom().end, quote(target))]
    for child in node.nodes("symbol"):
        atom = child.atom()
        if atom.value.startswith(old_name + "_"):
            changes.append((atom.start, atom.end, quote(new_name + atom.value[len(old_name):])))
    changed = update_properties(edits(symbol_text, changes), metadata(item))
    name_edits = []
    for child in parse(changed).nodes("symbol"):
        for pin in child.nodes("pin"):
            number = pin.one("number").atom().value
            if number in item.pin_names:
                atom = pin.one("name").atom()
                if atom.value != item.pin_names[number]:
                    name_edits.append((atom.start, atom.end, quote(item.pin_names[number])))
    changed = edits(changed, name_edits)
    if symbol_pins(changed) != symbol_pins(symbol_text):
        raise ValueError("Renaming a cache symbol changed its pin geometry")
    return raw(changed, parse(changed))


def load_library_symbol(lib_id, root):
    library, name = lib_id.split(":", 1)
    directory = Path(os.environ.get("KICAD9_SYMBOL_DIR", "C:/Program Files/KiCad/9.0/share/kicad/symbols"))
    path = root / "CodeSDR.kicad_sym" if library == "CodeSDR" else directory / (library + ".kicad_sym")
    source = path.read_text(encoding="utf-8")
    found = [s for s in parse(source).nodes("symbol") if s.atom().value == name]
    if len(found) != 1:
        raise ValueError(f"Cannot find {lib_id} in {path}")
    result = raw(source, found[0])
    atom = parse(result).atom()
    return edits(result, [(atom.start, atom.end, quote(lib_id))])


def make_instance(item, lib_id, symbol_text, x, y, root_uuid, mount_uuid, project):
    fields = {"Reference": item.ref, **metadata(item)}
    pieces = [f'(symbol (lib_id {quote(lib_id)}) (at {x:.4f} {y:.4f} 0) (unit 1)',
              '(exclude_from_sim no) (in_bom yes) (on_board yes)',
              f'(dnp {"yes" if getattr(item, "dnp", False) else "no"})',
              '(fields_autoplaced no)', f'(uuid "{uuid.uuid4()}")']
    for name, value in fields.items():
        px, py = (x+5.08, y-2.54) if name == "Reference" else (x+5.08, y+2.54)
        hide = "" if name in ("Reference", "Value") else " (hide yes)"
        pieces.append(f'(property {quote(name)} {quote(value)} (at {px:.4f} {py:.4f} 0) '
                      f'(effects (font (size 1.27 1.27)) (justify left){hide}))')
    for pin in symbol_pins(symbol_text):
        pieces.append(f'(pin {quote(pin)} (uuid "{uuid.uuid4()}"))')
    pieces.append(f'(instances (project {quote(project)} (path "/{root_uuid}/{mount_uuid}" '
                  f'(reference {quote(item.ref)}) (unit 1))))')
    return "\n\t".join(pieces) + "\n)"


def marker(net, point, marker_uuid=None, angle=0):
    x, y = point
    ident = marker_uuid or str(uuid.uuid4())
    if net is None:
        return f'(no_connect (at {x:.4f} {y:.4f}) (uuid "{ident}"))'
    result = (f'(global_label {quote(net)} (shape bidirectional) (at {x:.4f} {y:.4f} 0) '
            f'(fields_autoplaced yes) (effects (font (size 1.27 1.27)) (justify left)) (uuid "{ident}") '
            f'(property "Intersheetrefs" "${{INTERSHEET_REFS}}" (at {x+2.54:.4f} {y:.4f} 0) '
            '(effects (font (size 1.27 1.27)) (justify left) (hide yes))))')
    return orient_global_label(result, angle)


def at_point(node):
    at = node.one("at")
    return tuple(round(float(at.atom(i).value), 4) for i in (1, 2))


def free_position(occupied):
    # Match the existing seven-column A1 drawing grid. Conservative cell boxes
    # include the new passives' labels and keep additions inside the page frame.
    for row in range(10):
        for column in range(7):
            x = round(round((70 + 110*column)/1.27)*1.27, 4)
            y = round(round((90 + 52*row)/1.27)*1.27, 4)
            candidate = (x-45, y-20, x+45, y+20)
            if all(candidate[2] < box[0] or box[2] < candidate[0] or
                   candidate[3] < box[1] or box[3] < candidate[1] for box in occupied):
                occupied.append(candidate)
                return x, y
    raise ValueError("No free existing-grid location on A1 sheet")


def sync(root=None, items=None, *, write=False, replace_symbols=()):
    root = Path(root or design_model.ROOT).resolve()
    items = list(design_model.components if items is None else items)
    model = {item.ref: item for item in items}
    replace_symbols = set(replace_symbols)
    if replace_symbols - set(model):
        raise ValueError(f'Unknown symbol replacement: {sorted(replace_symbols-set(model))}')
    # Match generate_symbols' last-instance selection for shared custom-symbol
    # defaults; instance properties still retain their per-component metadata.
    canonical_custom = {desired_lib(i): i for i in items if desired_lib(i).startswith("CodeSDR:")}
    if len(model) != len(items):
        raise ValueError("Duplicate component reference in model")
    root_path = root / "Code-SDR-V2.kicad_sch"
    root_source = root_path.read_text(encoding="utf-8")
    top = parse(root_source)
    root_uuid = top.one("uuid").atom().value
    mounts, sources, parsed, libraries, global_cache = {}, {}, {}, {}, {}
    for sheet in top.nodes("sheet"):
        filename = property_value(sheet, "Sheetfile")
        path = (root / filename).resolve()
        if not path.is_relative_to(root):
            raise ValueError(f"Sheet escapes root: {filename}")
        key = path.stem
        if key in mounts:
            raise ValueError("Repeated sheet instances are not supported")
        mounts[key] = (path, sheet.one("uuid").atom().value)
        sources[key] = path.read_text(encoding="utf-8")
        parsed[key] = parse(sources[key])
        if any(parsed[key].nodes(k) for k in ("wire", "bus", "bus_entry", "sheet")):
            raise ValueError(f"{key}: requires label-only, non-nested generated sheets")
        library = parsed[key].one("lib_symbols")
        libraries[key] = {n.atom().value: raw(sources[key], n) for n in library.nodes("symbol")}
        for lib_id, definition in libraries[key].items():
            if lib_id in global_cache and symbol_pins(global_cache[lib_id]) != symbol_pins(definition):
                raise ValueError(f"Inconsistent cache geometry for {lib_id}")
            global_cache[lib_id] = definition
    if set(item.sheet for item in items) - set(mounts):
        raise ValueError("Model refers to a sheet absent from the preserved hierarchy")
    cache_users = {}
    for tree in parsed.values():
        for node in tree.nodes('symbol'):
            cache_users.setdefault(node.one('lib_id').atom().value, set()).add(property_value(node, 'Reference'))
    if replace_symbols - set().union(*cache_users.values()):
        raise ValueError('Symbol replacement requires an existing component')
    refs_seen, changed_refs, added_refs, pending = set(), [], [], {}
    path_map = {"root_uuid": root_uuid, "sheets": {k:v[1] for k,v in mounts.items()}, "components": {}}
    counts = {"net_labels_changed": 0, "nc_transitions": 0, "labels_added": 0, "labels_removed": 0,
              "labels_formatted": 0}
    geometry_replaced = []
    for key, (path, mount_uuid) in mounts.items():
        source, tree = sources[key], parsed[key]
        changes, additions, cache_additions, cache_updates, expected_points, occupied = [], [], {}, {}, {}, []
        label_angles = {}
        existing_marker_points = {at_point(node) for kind in ('global_label', 'no_connect', 'label')
                                  for node in tree.nodes(kind)}
        # Reserve raw markers before placing new instances. Their ownership
        # cannot be inferred from a new pin landing at the same coordinate.
        occupied.extend((x-2.54, y-2.54, x+2.54, y+2.54) for x, y in existing_marker_points)
        definitions = dict(libraries[key])
        original_positions, original_owners, replaced_old_points = {}, {}, set()
        for node in tree.nodes('symbol'):
            ref = property_value(node, 'Reference')
            original_positions[ref] = pin_positions(raw(source, node), libraries[key][node.one('lib_id').atom().value])
            for point in original_positions[ref].values():
                original_owners.setdefault(point, set()).add(ref)
        instances = []
        for node in tree.nodes("symbol"):
            ref = property_value(node, "Reference")
            if ref in refs_seen or ref not in model:
                raise ValueError(f"Duplicate or unmodelled existing reference: {ref}")
            refs_seen.add(ref)
            item = model[ref]
            if item.sheet != key:
                raise ValueError(f"Refusing to move {ref} to a different sheet")
            instance = raw(source, node)
            old_id, target = node.one("lib_id").atom().value, desired_lib(item)
            old_definition = libraries[key][old_id]
            if ref in replace_symbols:
                if not target.startswith('CodeSDR:'):
                    raise ValueError(f'Explicit pin geometry replacement requires a custom symbol: {ref}')
                import generate_symbols
                new_definition = generate_symbols.child_symbol(item)
                new_definition = renamed_symbol(new_definition, target, item)
                if (target in global_cache and symbol_pins(global_cache[target]) != symbol_pins(new_definition)
                        and cache_users.get(target, set()) - replace_symbols):
                    raise ValueError(f'Replacement changes an unapproved shared cache symbol: {target}')
                definitions[target] = new_definition
                if target in libraries[key]:
                    cache_updates[target] = new_definition
                else:
                    cache_additions[target] = new_definition
                local = parse(instance).one('lib_id').atom()
                instance = edits(instance, [(local.start, local.end, quote(target))])
                instance_tree = parse(instance)
                pin_nodes = {pin.atom().value:pin for pin in instance_tree.nodes('pin')}
                if len(pin_nodes) != len(instance_tree.nodes('pin')):
                    raise ValueError(f'Duplicate instance pin: {ref}')
                pin_edits = [(pin.start, pin.end, '') for number, pin in pin_nodes.items() if number not in item.pins]
                new_pins = [f'(pin {quote(number)} (uuid "{uuid.uuid4()}"))' for number in item.pins if number not in pin_nodes]
                if new_pins:
                    pin_edits.append((instance_tree.end-1, instance_tree.end-1, '\n\t'+'\n\t'.join(new_pins)+'\n'))
                instance = edits(instance, pin_edits)
                positions = pin_positions(instance, new_definition)
                if positions != original_positions[ref]:
                    geometry_replaced.append(ref)
                    replaced_old_points.update(original_positions[ref].values())
                    for point in positions.values():
                        if (point in existing_marker_points and ref not in original_owners.get(point, set())) or original_owners.get(point, set()) - {ref}:
                            raise ValueError(f'Replacement {ref} collides with a preserved pin or marker at {point}')
            elif old_id != target:
                if not (old_id.startswith("CodeSDR:") and target.startswith("CodeSDR:")):
                    raise ValueError(f"Non-custom symbol replacement requires review: {ref}")
                new_definition = renamed_symbol(old_definition, target, canonical_custom[target])
                if target in definitions and symbol_pins(definitions[target]) != symbol_pins(new_definition):
                    raise ValueError(f"Cache target collision: {target}")
                if target not in definitions:
                    definitions[target] = new_definition
                    cache_additions[target] = new_definition
                local = parse(instance).one("lib_id").atom()
                instance = edits(instance, [(local.start, local.end, quote(target))])
            if target.startswith("CodeSDR:") and ref not in replace_symbols:
                definitions[target] = renamed_symbol(definitions[target], target, canonical_custom[target])
                if target in libraries[key]:
                    cache_updates[target] = definitions[target]
                else:
                    cache_additions[target] = definitions[target]
            instance = update_properties(instance, metadata(item), dnp=getattr(item, "dnp", False))
            if ref in replace_symbols:
                instance = format_custom_instance(instance, definitions[target])
                label_angles.update(custom_label_angles(instance, definitions[target]))
            if instance != raw(source, node):
                changes.append((node.start, node.end, instance))
                changed_refs.append(ref)
            positions = pin_positions(instance, definitions[target])
            if set(positions) != set(item.pins):
                raise ValueError(f"Pin set changed for {ref}: cache {sorted(positions)} / model {sorted(item.pins)}")
            xs, ys = zip(*positions.values())
            cx, cy = at_point(parse(instance))
            occupied.append((min(min(xs)-12, cx-25), min(min(ys)-8, cy-18),
                             max(max(xs)+32, cx+25), max(max(ys)+8, cy+18)))
            instances.append((item, instance, positions))
        for item in (item for item in items if item.sheet == key and item.ref not in refs_seen):
            target = desired_lib(item)
            if target not in definitions:
                definition = global_cache.get(target)
                if definition is None:
                    if target.startswith("CodeSDR:"):
                        import generate_symbols
                        definition = generate_symbols.child_symbol(canonical_custom[target])
                        definition = renamed_symbol(definition, target, canonical_custom[target])
                    else:
                        definition = load_library_symbol(target, root)
                definitions[target] = definition
                cache_additions[target] = definition
            x, y = free_position(occupied)
            instance = make_instance(item, target, definitions[target], x, y, root_uuid, mount_uuid, root_path.stem)
            if target.startswith('CodeSDR:'):
                instance = format_custom_instance(instance, definitions[target])
                label_angles.update(custom_label_angles(instance, definitions[target]))
            positions = pin_positions(instance, definitions[target])
            if set(positions) != set(item.pins):
                raise ValueError(f"Pin set mismatch for new {item.ref}")
            if set(positions.values()) & existing_marker_points:
                raise ValueError(f"New component {item.ref} collides with a preserved marker")
            additions.append(instance)
            instances.append((item, instance, positions))
            refs_seen.add(item.ref)
            added_refs.append(item.ref)
        for item, instance, positions in instances:
            instance_uuid = parse(instance).one("uuid").atom().value
            path_map["components"][item.ref] = {
                "sheet": key, "sheet_uuid": mount_uuid, "component_uuid": instance_uuid,
                "path": f"/{mount_uuid}/{instance_uuid}", "value": item.value,
                "footprint": item.footprint, "dnp": bool(getattr(item, "dnp", False)),
            }
            for pin, point in positions.items():
                net = item.pins[pin]
                if point in expected_points and expected_points[point] != net:
                    raise ValueError(f"Conflicting nets at {key}:{point}")
                expected_points[point] = net
        label_nodes = {}
        for node in tree.nodes("global_label") + tree.nodes("no_connect") + tree.nodes("label"):
            label_nodes.setdefault(at_point(node), []).append(node)
        for point in replaced_old_points - set(expected_points):
            found = label_nodes.get(point, [])
            if len(found) > 1 or (found and found[0].head == 'label'):
                raise ValueError(f'Ambiguous old marker at replaced symbol pin: {key}:{point}')
            if found:
                changes.append((found[0].start, found[0].end, ''))
                counts['labels_removed'] += 1
        for point, net in expected_points.items():
            found = label_nodes.get(point, [])
            if len(found) > 1:
                raise ValueError(f"Multiple existing labels/NCs at {key}:{point}")
            if found:
                node = found[0]
                if node.head == "label":
                    raise ValueError("Local label at a model-owned pin requires explicit scope review")
                old_net = None if node.head == "no_connect" else node.atom().value
                if node.head == "global_label" and net is not None:
                    label = raw(source, node)
                    if old_net != net:
                        atom = parse(label).atom()
                        label = edits(label, [(atom.start, atom.end, quote(net))])
                        counts["net_labels_changed"] += 1
                    if point in label_angles:
                        oriented = orient_global_label(label, label_angles[point])
                        counts['labels_formatted'] += oriented != label
                        label = oriented
                    if label != raw(source, node):
                        changes.append((node.start, node.end, label))
                elif old_net != net:
                    changes.append((node.start, node.end, marker(net, point, node.one("uuid").atom().value,
                                                               label_angles.get(point, 0))))
                    counts["nc_transitions"] += 1
            else:
                additions.append(marker(net, point, angle=label_angles.get(point, 0)))
                counts["labels_added"] += 1
        if cache_additions:
            lib = tree.one("lib_symbols")
            changes.append((lib.end-1, lib.end-1, "\n\t\t" + "\n\t\t".join(cache_additions.values()) + "\n\t"))
        for node in tree.one("lib_symbols").nodes("symbol"):
            target = node.atom().value
            if target in cache_updates and cache_updates[target] != raw(source, node):
                changes.append((node.start, node.end, cache_updates[target]))
        if additions:
            changes.append((tree.end-1, tree.end-1, "\n\t" + "\n\t".join(additions) + "\n"))
        result = edits(source, changes)
        parse(result)
        if result != source:
            pending[path] = result
    if refs_seen != set(model):
        raise ValueError("Not all model components were represented")
    map_path = root / "build/schematic_paths.json"
    pending[map_path] = json.dumps(path_map, indent=2) + "\n"
    pending = {p:s for p,s in pending.items() if not p.exists() or p.read_text(encoding="utf-8") != s}
    report = {"mode": "write" if write else "dry-run", "root_uuid": root_uuid,
              "sheets": len(mounts), "components": len(model), "changed_components": changed_refs,
              "added_components": added_refs, "replaced_symbols": geometry_replaced, **counts,
              "changed_files": [str(p.relative_to(root)) for p in pending]}
    if write and pending:
        # All parsing/planning has succeeded before any production file changes.
        # Refuse a concurrent editor change instead of overwriting it.
        if root_path.read_text(encoding="utf-8") != root_source:
            raise ValueError("Root schematic changed during planning")
        for key, (path, _) in mounts.items():
            if path.read_text(encoding="utf-8") != sources[key]:
                raise ValueError(f"Concurrent schematic edit: {path}")
        stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S%fZ")
        backup = root / "build/schematic-sync-backups" / stamp
        for path in pending:
            if path.exists():
                target = backup / path.relative_to(root)
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(path, target)
        for path, result in pending.items():
            path.parent.mkdir(parents=True, exist_ok=True)
            temporary = path.with_name(path.name + ".sync-tmp")
            temporary.write_text(result, encoding="utf-8")
            temporary.replace(path)
        report["backup"] = str(backup)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=design_model.ROOT)
    parser.add_argument("--write", action="store_true", help="Apply planned edits and write actual hierarchy map")
    parser.add_argument('--replace-symbol', action='append', default=[], help='Explicitly rebuild this existing custom symbol pin geometry')
    args = parser.parse_args()
    print(json.dumps(sync(args.root, write=args.write, replace_symbols=args.replace_symbol), indent=2))


if __name__ == "__main__":
    main()
