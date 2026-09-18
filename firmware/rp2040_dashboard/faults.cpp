// ============================================================================
// faults.cpp - see faults.h
// ============================================================================
#include "faults.h"

#include <hardware/watchdog.h>
#include <hardware/adc.h>

fault_state_t g_faults;

static const char *k_names[FAULT_COUNT] = {
    "FPGA_CONFIG", "SPI_BUS", "LINK_DOWN", "OVERRUN", "OVERTEMP", "PHY_ID"
};

// link-down grace: a link blip during renegotiation must not raise a fault
static const uint32_t LINK_DOWN_GRACE_MS = 3000;
static uint32_t link_down_since = 0;

// RP2040 internal temperature sensor threshold
static const float OVERTEMP_C = 80.0f;

void faults_init(void) {
    memset(&g_faults, 0, sizeof(g_faults));
    adc_init();
    adc_set_temp_sensor_enabled(true);
}

void fault_raise(uint32_t bit) {
    if (!(g_faults.active & bit)) {
        uint32_t idx = __builtin_ctz(bit);
        if (idx < FAULT_COUNT)
            g_faults.count[idx]++;
    }
    g_faults.active  |= bit;
    g_faults.latched |= bit;
}

void fault_clear(uint32_t bit) {
    g_faults.active &= ~bit;
}

static float read_temp_c(void) {
    adc_select_input(4);                 // RP2040 internal temp sensor
    uint16_t raw = adc_read();
    float v = raw * 3.3f / 4096.0f;
    return 27.0f - (v - 0.706f) / 0.001721f;
}

void faults_tick(bool link_up, uint32_t dropped_words) {
    // temperature
    g_faults.temp_c = read_temp_c();
    if (g_faults.temp_c > OVERTEMP_C)
        fault_raise(FAULT_OVERTEMP);
    else
        fault_clear(FAULT_OVERTEMP);

    // link, with a grace period so renegotiation does not trip it
    uint32_t now = millis();
    if (link_up) {
        link_down_since = 0;
        fault_clear(FAULT_LINK_DOWN);
    } else {
        if (link_down_since == 0)
            link_down_since = now;
        else if ((now - link_down_since) > LINK_DOWN_GRACE_MS)
            fault_raise(FAULT_LINK_DOWN);
    }

    // FPGA overrun: the drop counter advancing means the link cannot carry the
    // configured bit depth and sample rate (see GATEWARE.md's rate table)
    static uint32_t last_dropped = 0;
    if (dropped_words != last_dropped) {
        fault_raise(FAULT_OVERRUN);
        last_dropped = dropped_words;
    } else {
        fault_clear(FAULT_OVERRUN);
    }
}

bool faults_check_phy_id(uint16_t id1, uint16_t id2) {
    // KSZ9031: PHYID1 = 0x0022, PHYID2 = 0x1620 (revision nibble masked)
    bool ok = (id1 == 0x0022) && ((id2 & 0xFFF0) == 0x1620);
    if (!ok)
        fault_raise(FAULT_PHY_ID);
    return ok;
}

void faults_report(void) {
    Serial.printf("FAULTS:active=0x%02x latched=0x%02x temp=%.1f",
                  (unsigned)g_faults.active, (unsigned)g_faults.latched,
                  g_faults.temp_c);
    for (uint32_t i = 0; i < FAULT_COUNT; i++) {
        if (g_faults.latched & (1u << i))
            Serial.printf(" %s=%u", k_names[i], (unsigned)g_faults.count[i]);
    }
    Serial.println();
}

void faults_watchdog_init(uint32_t timeout_ms) {
    watchdog_enable(timeout_ms, true);
}

void faults_watchdog_kick(void) {
    watchdog_update();
}
