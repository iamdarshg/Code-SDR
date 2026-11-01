"""Independent quasi-static microstrip (Hammerstad-Jensen) analysis for
Code-SDR V2 RF50 controlled-impedance nets.

What this IS: closed-form analytical transmission-line estimates for the
nominal RF50 geometry documented in FABRICATOR_IMPEDANCE_NOTE.md /
STACKUP_DRAWING.md (F.Cu w=0.23 mm, h=0.13 mm prepreg, t=0.035 mm, Dk=4.2).

What this is NOT: full-wave EM (no openEMS/CSXCAD/HFSS available on this
machine - see ENVIRONMENT.md), no measured S-parameters/VNA data.
Dispersion beyond the quasi-static limit and all 3-D effects (launches,
vias, solder mask, reference-plane splits) are NOT captured here.

Formulas: Hammerstad-Jensen (w/h >= 1 case) with Hammerstad thickness
correction, per R. E. Collin / Wadell "Transmission Line Design Handbook".
"""

from __future__ import annotations

import json
import math
from pathlib import Path

import numpy as np

OUT = Path(__file__).resolve().parents[1] / "outputs"
PLOT = Path(__file__).resolve().parents[1] / "plots"

# Nominal V2 RF50 design point (documented values).
W_MM = 0.23        # nominal trace width
H_MM = 0.13        # outer prepreg thickness
T_MM = 0.035       # finished copper thickness
ER = 4.2           # model Dk
C0 = 299_792_458.0
MU0 = 4e-7 * math.pi
SIGMA_CU = 5.8e7   # bulk copper conductivity (model)
TAND = 0.020       # FR-4 loss tangent model (typical, unverified for the
                   # selected fabricator's laminate; field-solve required)


def w_over_h(w_mm: float, h_mm: float) -> float:
    return w_mm / h_mm


def dk_eff(er: float, wh: float) -> float:
    """Hammerstad-Jensen effective permittivity (fits w/h in 0.1..3)."""
    return (er + 1.0) / 2.0 + (er - 1.0) / 2.0 * (1.0 + 12.0 / wh) ** -0.5


def thickness_correction(w_mm: float, t_mm: float) -> float:
    """Hammerstad strip-thickness correction (w/h >= 1/(2*pi) branch)."""
    return t_mm / math.pi * (1.0 + math.log(4.0 * math.pi * w_mm / t_mm))


def z0_quasi(w_mm: float, h_mm: float, t_mm: float, er: float) -> dict:
    """Return quasi-static Z0 and companions for a single-ended outer
    microstrip. Uses the corrected effective width."""
    wh = w_mm / h_mm
    dw = 0.0 if t_mm <= 0 else thickness_correction(w_mm, t_mm)
    we = w_mm + dw
    whe = we / h_mm
    ee = dk_eff(er, whe)
    if whe >= 1.0:
        z = (120.0 * math.pi) / (math.sqrt(ee) * (whe + 1.393 + 0.667 * math.log(whe + 1.444)))
    else:
        z = (60.0 / math.sqrt(ee)) * math.log(8.0 / whe + whe / 4.0)
    return {"w_mm": w_mm, "h_mm": h_mm, "t_mm": t_mm, "er": er,
            "w_eff_mm": we, "w_h": wh, "dk_eff": ee, "z0_ohm": z}


def skin_depth_m(freq_hz: float, sigma: float = SIGMA_CU) -> float:
    return 1.0 / math.sqrt(math.pi * freq_hz * MU0 * sigma)


def wavelength_mm(freq_hz: float, dkeff: float) -> float:
    return C0 / (freq_hz * math.sqrt(dkeff)) * 1e3


def attenuation_dbm(freq_hz: float, z0: float, dkeff: float, w_mm: float,
                    h_mm: float, t_mm: float) -> dict:
    """Analytical (quasi-static) microstrip attenuation per 10 mm.

    Conductor part: Hammerstad alpha_c = Rs/(Z0*w') with surface roughness
    omitted (Rs = skin-surface resistance). Dielectric part:
    alpha_d = pi*f*sqrt(dkeff)*tanD/c0. Model-grade estimates only.
    """
    rs = 1.0 / (sigma_cu() * skin_depth_m(freq_hz)) if False else skin_depth_m(freq_hz) / (SIGMA_CU * skin_depth_m(freq_hz))
    # Rs = 1/(sigma*delta) = delta*rho/delta^2 ... == rho/delta
    rs = 1.0 / (SIGMA_CU * skin_depth_m(freq_hz))
    w_m = w_mm * 1e-3
    # corrected width for conduction
    dw = 0.0 if t_mm <= 0 else thickness_correction(w_mm, t_mm)
    wp = w_mm * 1e-3 + dw * 1e-3
    alpha_c_np = rs / (z0 * wp)          # Np/m, thin-strip approx
    alpha_c_db_per_10mm = alpha_c_np * 8.6858896 * 0.010
    alpha_d_np = math.pi * freq_hz * math.sqrt(dkeff) * TAND / C0
    alpha_d_db_per_10mm = alpha_d_np * 8.6858896 * 0.010
    return {"freq_hz": freq_hz, "skin_depth_um": skin_depth_m(freq_hz) * 1e6,
            "alpha_c_db_10mm": alpha_c_db_per_10mm,
            "alpha_d_db_10mm": alpha_d_db_per_10mm,
            "alpha_total_db_10mm": alpha_c_db_per_10mm + alpha_d_db_per_10mm}


def sigma_cu() -> float:
    return SIGMA_CU


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    PLOT.mkdir(parents=True, exist_ok=True)

    nominal = z0_quasi(W_MM, H_MM, T_MM, ER)
    print("=== NOMINAL RF50 POINT (w=0.23 mm, h=0.13 mm, t=0.035 mm, Dk=4.2) ===")
    for k, v in nominal.items():
        print(f"  {k:10s} {v:.6g}")

    # Sensitivity sweeps ---------------------------------------------------
    widths = [0.18, 0.20, 0.21, 0.22, 0.23, 0.24, 0.25, 0.26, 0.28, 0.30]
    dk_vals = [3.80, 3.90, 4.00, 4.10, 4.20, 4.30, 4.40, 4.50, 4.60]
    hs = [0.10, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16]

    width_rows = [z0_quasi(w, H_MM, T_MM, ER) for w in widths]
    dk_rows = [z0_quasi(W_MM, H_MM, T_MM, dk) for dk in dk_vals]
    h_rows = [z0_quasi(W_MM, h, T_MM, ER) for h in hs]

    # Nonconforming widths observed on the committed board (0.15/0.10 mm).
    alt_rows = {
        "0.15 mm (observed on board)": z0_quasi(0.15, H_MM, T_MM, ER),
        "0.10 mm (observed on board)": z0_quasi(0.10, H_MM, T_MM, ER),
    }

    # Frequency-dependent checks at useful points --------------------------
    freqs = [100e6, 500e6, 1e9, 2.436e9, 5e9, 8e9, 10e9]
    atten = [attenuation_dbm(f, nominal["z0_ohm"], nominal["dk_eff"], W_MM, H_MM, T_MM) for f in freqs]
    wave = [wavelength_mm(f, nominal["dk_eff"]) for f in freqs]

    print("\n=== QUASI-STATIC ATTENUATION / SKIN DEPTH (model grades) ===")
    print(" freq   skin(um)  aC(dB/10mm) aD(dB/10mm) aTot(dB/10mm)  lambda_eff(mm)")
    for a, lam in zip(atten, wave):
        print(f" {a['freq_hz']/1e9:6.3f}G   {a['skin_depth_um']:7.3f}   {a['alpha_c_db_10mm']:9.4f}   {a['alpha_d_db_10mm']:9.4f}   {a['alpha_total_db_10mm']:9.4f}   {lam:9.3f}")

    # Electrical length of the longest RF50 net (95.44 mm) and B.Cu nets.
    max_len_mm = 95.439
    print("\n=== ELECTRICAL LENGTH (quasi-static wavelength basis) ===")
    print(" net length   @100MHz   @2.436GHz  @10GHz")
    for label, ll in (("max RF50 net 95.44mm", max_len_mm), ("RF_HD_LNA_OUT", 28.0)):
        pass
    for f, lam in zip(freqs[:5], wave[:5]):
        print(f"  {max_len_mm:6.1f} mm vs lambda_eff {lam:8.2f} mm -> {max_len_mm/lam:.3f} lambda at {f/1e6:.0f} MHz")

    payload = {
        "nominal": nominal,
        "sweep_width": width_rows,
        "sweep_dk": dk_rows,
        "sweep_h": h_rows,
        "nonconforming_widths": alt_rows,
        "attenuation": atten,
        "wavelength_mm": wave,
        "note": "ANALYTICAL QUASI-STATIC ONLY. No full-wave solver, no measurements. "
                "Solder mask and 3-D effects not modeled. Field-solve required at fabricator.",
        "model": "Hammerstad-Jensen + Hammerstad thickness correction; "
                 "tanD=0.02 model; sigma_cu=5.8e7 S/m",
    }
    (OUT / "microstrip_quasistatic.json").write_text(json.dumps(payload, indent=1), encoding="utf-8")
    print(f"\nWrote {OUT / 'microstrip_quasistatic.json'}")

    # Minimal matplotlib figure (system python has it; kicad python does not)
    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
        fig, axes = plt.subplots(2, 2, figsize=(12, 8))
        axes[0][0].plot([r["w_mm"] for r in width_rows], [r["z0_ohm"] for r in width_rows], "o-")
        axes[0][0].axhline(50, color="k", ls="--", lw=0.8)
        axes[0][0].set(xlabel="width (mm)", ylabel="Z0 (ohm)", title="Z0 vs width (h=0.13, t=0.035, Dk=4.2)")
        axes[0][1].plot([r["er"] for r in dk_rows], [r["z0_ohm"] for r in dk_rows], "s-")
        axes[0][1].axhline(50, color="k", ls="--", lw=0.8)
        axes[0][1].set(xlabel="Dk", ylabel="Z0 (ohm)", title="Z0 vs Dk (w=0.23, h=0.13)")
        axes[1][0].plot([r["h_mm"] for r in h_rows], [r["z0_ohm"] for r in h_rows], "^-")
        axes[1][0].axhline(50, color="k", ls="--", lw=0.8)
        axes[1][0].set(xlabel="h (mm)", ylabel="Z0 (ohm)", title="Z0 vs prepreg h (w=0.23, Dk=4.2)")
        fs = [a["freq_hz"] / 1e9 for a in atten]
        axes[1][1].plot(fs, [a["alpha_total_db_10mm"] for a in atten], "o-")
        axes[1][1].set(xlabel="freq (GHz)", ylabel="aTotal dB/10mm",
                       title="Model attenuation (per 10 mm, quasi-static)")
        fig.suptitle("Code-SDR V2 RF50 quasi-static microstrip (analytical, not full-wave)")
        fig.tight_layout()
        fig.savefig(PLOT / "microstrip_quasistatic.png", dpi=110)
        print("plot:", PLOT / "microstrip_quasistatic.png")
    except Exception as exc:  # pragma: no cover
        print("plot skipped:", exc)


if __name__ == "__main__":
    main()