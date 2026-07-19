"""Generate the modular KiCad 9 V2 schematic hierarchy."""

from __future__ import annotations

import json
import uuid
from pathlib import Path

import kicad_sch_api as ksa
from kicad_sch_api.library.cache import get_symbol_cache

from design_model import ROOT, SHEETS, by_sheet
import generate_symbols


SHEETS_DIR = ROOT / "sheets"
MAP_PATH = ROOT / "build" / "schematic_paths.json"


def add_pin_net(sch, ref: str, pin: str, net: str | None) -> None:
    position = sch.get_component_pin_position(ref, pin)
    if net is None:
        sch.no_connects.add(position=position)
    else:
        # kicad-sch-api 0.6 currently accepts global labels but its serializer
        # drops them.  Return the position so they can be emitted explicitly.
        return net, position
    return None


def inject_global_labels(path: Path, labels: list[tuple[str, object]]) -> None:
    """Insert KiCad 9 global-label S-expressions after API serialization."""
    blocks = []
    for text, position in labels:
        label_uuid = str(uuid.uuid4())
        blocks.append(
            f'''\
\t(global_label "{text}"
\t\t(shape bidirectional)
\t\t(at {position.x:.4f} {position.y:.4f} 0)
\t\t(fields_autoplaced yes)
\t\t(effects
\t\t\t(font (size 1.27 1.27))
\t\t\t(justify left)
\t\t)
\t\t(uuid "{label_uuid}")
\t\t(property "Intersheetrefs" "${{INTERSHEET_REFS}}"
\t\t\t(at {position.x + 2.54:.4f} {position.y:.4f} 0)
\t\t\t(effects
\t\t\t\t(font (size 1.27 1.27))
\t\t\t\t(justify left)
\t\t\t\t(hide yes)
\t\t\t)
\t\t)
\t)'''
        )
    source = path.read_text(encoding="utf-8")
    source = source.rstrip()
    if not source.endswith(")"):
        raise RuntimeError(f"Unexpected KiCad schematic terminator in {path}")
    source = source[:-1] + "\n" + "\n".join(blocks) + "\n)\n"
    path.write_text(source, encoding="utf-8")


def main() -> None:
    ROOT.mkdir(parents=True, exist_ok=True)
    SHEETS_DIR.mkdir(parents=True, exist_ok=True)
    MAP_PATH.parent.mkdir(parents=True, exist_ok=True)
    generate_symbols.main()
    cache = get_symbol_cache()
    cache.add_library_path(generate_symbols.LIB)

    root = ksa.create_schematic("Code-SDR-V2")
    root.set_paper_size("A3")
    root.set_title_block(
        title="Code-SDR V2 - 100 MHz to 10 GHz Cost-First Receiver",
        rev="2.0",
        company="Code-SDR",
        comments={
            1: "Modular reference design; controlled RF stackup and bench VNA validation required.",
            2: "Control/SDR board only. ESC remains a separate external board.",
        },
    )
    sheet_uuids = {}
    for index, (sheet, title) in enumerate(SHEETS):
        x = 25 + (index % 2) * 120
        y = 25 + (index // 2) * 45
        uuid = root.add_sheet(
            name=title,
            filename=f"sheets/{sheet}.kicad_sch",
            position=(x, y),
            size=(100, 30),
            page_number=str(index + 2),
        )
        sheet_uuids[sheet] = uuid
        root.add_text(
            f"{sheet}: {title}",
            # Free text emitted by kicad-sch-api is center-justified.  Keep the
            # complete functional-sheet caption inside its 100 mm sheet box.
            position=(x + 50, y + 18),
            size=1.27,
        )
    root.save(ROOT / "Code-SDR-V2.kicad_sch")

    path_map = {"root_uuid": root.uuid, "sheets": {}, "components": {}}
    for index, (sheet, title) in enumerate(SHEETS):
        sch = ksa.create_schematic(sheet)
        # The design intentionally names every IC pad and emits explicit net
        # labels.  A1 keeps dense FPGA/PHY/PLL symbols and all 60 IF-sheet
        # parts inside the printable frame without label collisions.
        sch.set_paper_size("A1")
        sch.set_title_block(
            title=f"{index + 1:02d} - {title}",
            rev="2.0",
            company="Code-SDR",
            comments={
                1: "Every IC pad is named; DNC/NC pins are explicitly marked.",
                2: "Net labels are global only where signals cross sheet boundaries.",
            },
        )
        sch.add_text(
            f"{title}\nSee HARDWARE_VALIDATION.md and hardware/v2/README.md for design limits.",
            # Center the two-line heading far enough from the A2 left border
            # that the longer design-limit line is never clipped in PDF.
            position=(150, 25),
            size=1.5,
            bold=True,
        )
        sheet_items = by_sheet(sheet)
        labels = []
        cols = 7
        for item_index, item in enumerate(sheet_items):
            # Seven columns retain generous label clearance and limit the
            # largest sheet to nine rows above the title block.
            sx = 70 + (item_index % cols) * 110
            sy = 90 + (item_index // cols) * 52
            lib_id = item.symbol if item.symbol and ":" in item.symbol else f"CodeSDR:{item.symbol_name}"
            component = sch.components.add(
                lib_id=lib_id,
                reference=item.ref,
                value=item.value,
                position=(sx, sy),
                footprint=item.footprint,
                Datasheet=item.datasheet,
                Description=item.description,
            )
            for pin, net in item.pins.items():
                label = add_pin_net(sch, item.ref, pin, net)
                if label is not None:
                    labels.append(label)
            path_map["components"][item.ref] = {
                "sheet": sheet,
                "sheet_uuid": sheet_uuids[sheet],
                "component_uuid": component.uuid,
                "path": f"/{sheet_uuids[sheet]}/{component.uuid}",
                "value": item.value,
                "footprint": item.footprint,
            }
        sheet_path = SHEETS_DIR / f"{sheet}.kicad_sch"
        sch.save(sheet_path)
        inject_global_labels(sheet_path, labels)
        path_map["sheets"][sheet] = sheet_uuids[sheet]

    MAP_PATH.write_text(json.dumps(path_map, indent=2) + "\n", encoding="utf-8")
    print(f"Generated root and {len(SHEETS)} child schematics; {len(path_map['components'])} components")


if __name__ == "__main__":
    main()
