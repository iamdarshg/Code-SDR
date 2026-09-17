// ============================================================================
// fpga_config.h - reprogram the CrossLink FPGA from the RP2040 (slave SPI).
//
// Both bitstreams live on the RP2040's 2 MB QSPI flash (it is the board's only
// configuration flash - HARDWARE.md section 1). Selecting a mode reconfigures
// the FPGA in ~0.1-0.2 s with no host and no re-flash of the board.
//
// Wiring (HARDWARE.md section 8):
//   FPGA_SPI_SCK  -> U20.J1     FPGA_SPI_CS_N -> U20.H1
//   FPGA_SPI_MOSI -> U20.G1     FPGA_SPI_MISO -> U20.K1
//   FPGA_RESET_N  -> U20.H2     FPGA_CDONE    -> U20.J2
//
// NOTE: the byte-level details of the CrossLink sysCONFIG slave-SPI flow (lead
// dummy bytes, bit order, and the exact reset/init handshake) must be confirmed
// against "Lattice CrossLink Programming and Configuration User Guide"
// (FPGA-TN-02014) before flashing real hardware. The structure below is the
// standard Lattice slave-SPI sequence; only the timing constants are unverified.
// ============================================================================
#pragma once

#include <Arduino.h>

// Pin map - cross-check against the schematic and the RP2040 QFN-56 pin->GPIO
// mapping. These are physical PCB pins from the net list, not confirmed GPIOs.
#define PIN_FPGA_SCK     2
#define PIN_FPGA_MOSI    3
#define PIN_FPGA_MISO    4
#define PIN_FPGA_CS_N    5
#define PIN_FPGA_RESET_N 29
#define PIN_FPGA_CDONE   41

// Bitstream slots on the RP2040 flash filesystem.
#define BITSTREAM_RAW  "/raw.bit"
#define BITSTREAM_FFT  "/fft.bit"

// Bring the FPGA out of reset and stream `path` over slave SPI.
// Returns true if CDONE went high within the timeout.
bool fpga_program_from_file(const char *path);

// Convenience: select by mode index (0 = raw, 1 = fft).
bool fpga_select_mode(int mode);
