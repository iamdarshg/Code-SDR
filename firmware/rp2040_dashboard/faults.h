// ============================================================================
// faults.h - fault detection and reporting for the RP2040 controller.
//
// The RP2040 is the board's sole controller (HARDWARE.md section 1), so it owns
// fault handling. This module watches:
//
//   * FPGA configuration lost (CDONE drops after it was high) -> reconfigure
//   * SPI bus failure (telemetry reads all-ones / no response)
//   * PHY link down for longer than a grace period
//   * FPGA overrun (the drop counter in the telemetry header is advancing)
//   * Overtemperature (RP2040 internal sensor) - a proxy for the RF rail, which
//     dominates the ~6.6 W budget in POWER.md
//   * PHY identity mismatch at bring-up
//
// Each fault has an `active` bit (currently asserted) and a `latched` bit (ever
// asserted since boot) plus a counter, so a transient is visible after the fact.
// A hardware watchdog resets the controller if the main loop ever hangs.
// ============================================================================
#pragma once

#include <Arduino.h>

// Fault bits. Keep in sync with the FAULT_* strings in faults.cpp.
#define FAULT_FPGA_CONFIG (1u << 0)
#define FAULT_SPI_BUS     (1u << 1)
#define FAULT_LINK_DOWN   (1u << 2)
#define FAULT_OVERRUN     (1u << 3)
#define FAULT_OVERTEMP    (1u << 4)
#define FAULT_PHY_ID      (1u << 5)
#define FAULT_COUNT       6

typedef struct {
    uint32_t active;                       // currently asserted
    uint32_t latched;                      // ever asserted since boot
    uint32_t count[FAULT_COUNT];           // number of times raised
    float    temp_c;                       // last RP2040 temperature reading
} fault_state_t;

extern fault_state_t g_faults;

// call once at boot
void faults_init(void);

// raise/clear a fault bit (raising also latches and increments its counter)
void fault_raise(uint32_t bit);
void fault_clear(uint32_t bit);

// per-loop housekeeping: sample temperature, age the link-down timer
void faults_tick(bool link_up, uint32_t dropped_words);

// one-line machine-readable summary: "FAULTS:active=0x.. latched=0x.. temp=.."
void faults_report(void);

// hardware watchdog: reset the RP2040 if the main loop stops running
void faults_watchdog_init(uint32_t timeout_ms);
void faults_watchdog_kick(void);

// verify the PHY answers with the expected KSZ9031 identity
bool faults_check_phy_id(uint16_t id1, uint16_t id2);
