/*
 * Code-SDR V2 - RP2040 network/link dashboard
 * -------------------------------------------
 * The RP2040 is the board's controller (HARDWARE.md section 1) and owns the
 * SPI link to the FPGA (section 8). This firmware:
 *
 *   1. configures the datapath (mode, sample bits, decimation, UDP port)
 *   2. polls the FPGA telemetry block over SPI (v2_spi_regs / v2_telemetry)
 *   3. computes the link budget for the CURRENT bit depth and sample rate
 *   4. prints a dashboard, plus machine-readable TEL lines the SDR program reads
 *
 * The dashboard answers exactly the question asked: at this bit count and this
 * sample rate, how many samples/packets are being dropped, and is the link
 * oversubscribed?
 *
 * Build: Arduino-Pico core (Raspberry Pi Pico), Tools > USB Stack > "Pico SDK"
 * so Serial is USB CDC and the SDR program can read it.
 *
 * IMPORTANT: the four SPI GPIO numbers below are physical RP2040 pins from the
 * PCB net list (U10.x) and must be cross-checked against the schematic and the
 * RP2040 QFN-56 pin -> GPIO mapping before flashing. They are NOT guessed to be
 * correct; they are the single hardware fact this file cannot derive.
 */

#include <Arduino.h>
#include <SPI.h>
#include <LittleFS.h>
#include "fpga_config.h"
#include "faults.h"

// telemetry status register bits (see verilog/v2_telemetry.v)
#define STAT_LINK_UP     (1u << 0)
#define STAT_PLL_LOCK    (1u << 1)
#define STAT_PHY_ERROR   (1u << 20)

// ------------------------------------------------------------------- self-test
// Runs after configuration. Exercises every path we can from the controller:
// the SPI bus, the telemetry register file, and the PHY identity.
static void selftest(void) {
    // 1. FPGA configured?
    if (digitalRead(PIN_FPGA_CDONE) != HIGH) {
        Serial.println("# SELFTEST: FPGA CDONE low - not configured");
        fault_raise(FAULT_FPGA_CONFIG);
    }

    // 2. SPI bus: a telemetry read must come back with a magic-consistent value.
    //    A stuck bus reads all-ones or all-zeros.
    uint32_t status = fpga_read(REG_T_STATUS);
    if (status == 0xFFFFFFFF || status == 0x00000000) {
        Serial.printf("# SELFTEST: SPI bus read 0x%08X - bus or FPGA not responding\n",
                      (unsigned)status);
        fault_raise(FAULT_SPI_BUS);
    } else {
        Serial.printf("# SELFTEST: SPI bus OK (status=0x%08X)\n", (unsigned)status);
    }

    // 3. PHY bring-up: the FPGA's PHY manager sets its error flag if the KSZ9031
    //    identity check, reset sequence or auto-negotiation failed. That flag is
    //    exposed in the telemetry status register, so the controller can see it.
    if (status & STAT_PHY_ERROR) {
        Serial.println("# SELFTEST: FPGA reports PHY bring-up error");
        fault_raise(FAULT_PHY_ID);
    } else {
        Serial.println("# SELFTEST: PHY bring-up OK");
    }
    Serial.println("# SELFTEST: complete");
}

// ---------------------------------------------------------------- pin mapping
#define PIN_FPGA_SCK   2    // FPGA_SPI_SCK_MCU  U10.2
#define PIN_FPGA_MOSI  3    // FPGA_SPI_MOSI     U10.3
#define PIN_FPGA_MISO  4    // FPGA_SPI_MISO     U10.4
#define PIN_FPGA_CS_N  5    // FPGA_SPI_CS_N     U10.5
#define PIN_FPGA_RESET 29   // FPGA_RESET_N      U10.29
#define PIN_FPGA_CDONE 41   // FPGA_CDONE        U10.41

// ------------------------------------------------------------ FPGA registers
// writes (addr[7]=0)
#define REG_MODE      0x00
#define REG_BITS      0x01
#define REG_DECIM     0x02
#define REG_NCO       0x03
#define REG_ENABLE    0x04
#define REG_DST_PORT  0x05
// reads (addr[7]=1); telemetry block lives at 0x20..0x3F
#define REG_T_PACKETS 0xA0
#define REG_T_DROPPED 0xA4
#define REG_T_STICKY  0xA8
#define REG_T_STATUS  0xAC
#define REG_T_MBPS    0xB0
#define REG_T_SEQ     0xB4

// ------------------------------------------------------------- link constants
static const float ADC_HZ        = 100.0e6;  // REF_100M_ADC
static const float UDP_CEIL_MBPS = 957.0;    // 1472B payload / 1538B wire @1500 MTU

static uint8_t  g_bits  = 10;
static uint8_t  g_decim = 2;
static uint8_t  g_mode  = 0;                 // 0 = raw, 1 = on-FPGA FFT

static uint32_t prev_dropped = 0;
static uint32_t prev_ms      = 0;

// ------------------------------------------------------------------ SPI access
static uint32_t fpga_read(uint8_t a) {
    uint32_t v = 0;
    digitalWrite(PIN_FPGA_CS_N, LOW);
    SPI.transfer(a);                          // addr[7]=1 -> read
    for (int i = 0; i < 4; i++) v = (v << 8) | (uint8_t)SPI.transfer(0x00);
    digitalWrite(PIN_FPGA_CS_N, HIGH);
    return v;
}

static void fpga_write(uint8_t a, uint32_t v) {
    digitalWrite(PIN_FPGA_CS_N, LOW);
    SPI.transfer(a & 0x7F);
    SPI.transfer((v >> 24) & 0xFF);
    SPI.transfer((v >> 16) & 0xFF);
    SPI.transfer((v >>  8) & 0xFF);
    SPI.transfer( v        & 0xFF);
    digitalWrite(PIN_FPGA_CS_N, HIGH);
}

// ---------------------------------------------------------------------- setup
void setup() {
    Serial.begin(115200);
    // bitstream slots for mode switching live on the RP2040 flash
    if (!LittleFS.begin()) {
        Serial.println("# LittleFS mount failed; mode switching unavailable");
    }
    pinMode(PIN_FPGA_CS_N, OUTPUT);
    pinMode(PIN_FPGA_RESET, OUTPUT);
    digitalWrite(PIN_FPGA_CS_N, HIGH);

    // Release the FPGA and wait for CDONE.
    digitalWrite(PIN_FPGA_RESET, HIGH);
    pinMode(PIN_FPGA_CDONE, INPUT);
    uint32_t t0 = millis();
    while (!digitalRead(PIN_FPGA_CDONE) && (millis() - t0) < 2000) delay(10);

    SPI.setSCK(PIN_FPGA_SCK);
    SPI.setTX(PIN_FPGA_MOSI);
    SPI.setRX(PIN_FPGA_MISO);
    SPI.begin();

    // Default operating point: 10-bit, decimate by 2 (all bits, 50 MSPS).
    fpga_write(REG_BITS,  g_bits);
    fpga_write(REG_DECIM, g_decim);
    fpga_write(REG_MODE,  g_mode);
    fpga_write(REG_ENABLE, 1);

    Serial.println("# Code-SDR V2 dashboard. Commands: 'b <8|10>' 'd <1|2|4|8>' 'm <0|1>' '0'/'1' = swap bitstream");
    Serial.println("ms,bits,decim,mode,msps,payload_mbps,ceiling_mbps,headroom_mbps,required,packets,dropped,drops_per_s,sticky,link,pll");

    // fault handling + watchdog: reset the controller if the loop ever hangs
    faults_init();
    faults_watchdog_init(3000);
    selftest();
}

// ----------------------------------------------------------------------- loop
void loop() {
    faults_watchdog_kick();

    // FPGA configuration lost? CDONE should stay high in user mode.
    if (digitalRead(PIN_FPGA_CDONE) != HIGH) {
        fault_raise(FAULT_FPGA_CONFIG);
        Serial.println("# FPGA CDONE low - reconfiguring");
        if (g_mode == 1) fpga_select_mode(1); else fpga_select_mode(0);
    } else {
        fault_clear(FAULT_FPGA_CONFIG);
    }

    // Consume any commands from the SDR program / user.
    if (Serial.available()) {
        char c = Serial.read();
        long arg = Serial.parseInt();
        if      (c == 'b' && (arg == 8 || arg == 10)) { g_bits = arg;  fpga_write(REG_BITS, g_bits); }
        else if (c == 'd' && arg > 0)                 { g_decim = arg; fpga_write(REG_DECIM, g_decim); }
        else if (c == 'm')                            { g_mode = arg ? 1 : 0; fpga_write(REG_MODE, g_mode); }
        else if (c == '0' || c == '1') {
            // Full reconfiguration: load the other bitstream from flash and
            // reprogram the FPGA. No host needed, no re-flash of the board.
            int want = (c == '1') ? 1 : 0;
            Serial.printf("# selecting %s bitstream...\n", want ? "FFT" : "RAW");
            if (fpga_select_mode(want)) {
                g_mode = want;
                fpga_write(REG_BITS,  g_bits);
                fpga_write(REG_DECIM, g_decim);
                fpga_write(REG_MODE,  g_mode);
                fpga_write(REG_ENABLE, 1);
                Serial.printf("# mode switch complete\n");
            } else {
                Serial.printf("# mode switch FAILED\n");
            }
        }
    }

    static uint32_t last = 0;
    uint32_t now = millis();
    if (now - last < 500) return;             // 2 Hz dashboard
    last = now;

    uint32_t status  = fpga_read(REG_T_STATUS);
    uint32_t packets = fpga_read(REG_T_PACKETS);
    uint32_t dropped = fpga_read(REG_T_DROPPED);
    uint32_t sticky  = fpga_read(REG_T_STICKY);

    uint8_t link      = status & 0x01;
    uint8_t pll       = (status >> 1) & 0x01;
    uint8_t mode      = (status >> 2) & 0x03;
    uint8_t sticky_ov = (status >> 4) & 0x01;
    uint8_t bits      = (status >> 5) & 0x7F;
    uint8_t decim     = (status >> 12) & 0xFF;
    if (decim == 0) decim = 1;

    float msps          = (ADC_HZ / (float)decim) / 1.0e6;
    float payload_mbps  = msps * (float)bits;
    float headroom      = UDP_CEIL_MBPS - payload_mbps;
    uint8_t required    = (payload_mbps > UDP_CEIL_MBPS) ? 1 : 0;

    uint32_t dt_ms = now - prev_ms;
    uint32_t dd    = dropped - prev_dropped;
    float drops_per_s = (dt_ms > 0) ? (dd * 1000.0f / (float)dt_ms) : 0.0f;
    prev_dropped = dropped;
    prev_ms      = now;

    // Human dashboard.
    Serial.print("--- Code-SDR V2 link dashboard ---\r\n");
    Serial.print("  mode            : "); Serial.println(mode ? "on-FPGA FFT" : "raw (laptop FFT)");
    Serial.print("  bit depth       : "); Serial.print(bits); Serial.println(" bit");
    Serial.print("  sample rate     : "); Serial.print(msps, 2); Serial.println(" MSPS");
    Serial.print("  payload load    : "); Serial.print(payload_mbps, 1); Serial.println(" Mbps");
    Serial.print("  link ceiling    : "); Serial.print(UDP_CEIL_MBPS, 1); Serial.println(" Mbps (1500 MTU)");
    Serial.print("  headroom        : "); Serial.print(headroom, 1); Serial.print(" Mbps");
    Serial.println(required ? "   *** OVERSUBSCRIBED: drops guaranteed ***" : "   OK");
    Serial.print("  dropped words   : "); Serial.print(dropped);
    Serial.print("  ("); Serial.print(drops_per_s, 1); Serial.println(" /s)");
    Serial.print("  link / pll      : "); Serial.print(link); Serial.print(" / "); Serial.println(pll);

    // fault housekeeping + report
    faults_tick(link, dropped);
    faults_report();

    // Machine-readable line for the SDR program.
    Serial.print("TEL:");
    Serial.print(now);          Serial.print(',');
    Serial.print(bits);         Serial.print(',');
    Serial.print(decim);        Serial.print(',');
    Serial.print(mode);         Serial.print(',');
    Serial.print(msps, 3);      Serial.print(',');
    Serial.print(payload_mbps, 1); Serial.print(',');
    Serial.print(UDP_CEIL_MBPS, 1); Serial.print(',');
    Serial.print(headroom, 1);  Serial.print(',');
    Serial.print(required);     Serial.print(',');
    Serial.print(packets);      Serial.print(',');
    Serial.print(dropped);      Serial.print(',');
    Serial.print(drops_per_s, 1); Serial.print(',');
    Serial.print(sticky | sticky_ov); Serial.print(',');
    Serial.print(link);         Serial.print(',');
    Serial.println(pll);
}
