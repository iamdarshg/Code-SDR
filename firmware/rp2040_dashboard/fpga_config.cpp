// ============================================================================
// fpga_config.cpp - slave-SPI configuration of the LIF-MD6000 from the RP2040.
// ============================================================================
#include "fpga_config.h"

#include <LittleFS.h>
#include <SPI.h>

// ----------------------------------------------------------------- utilities
static bool wait_cdone(bool level, uint32_t timeout_ms) {
    uint32_t t0 = millis();
    while (digitalRead(PIN_FPGA_CDONE) != (level ? HIGH : LOW)) {
        if ((millis() - t0) > timeout_ms)
            return false;
        delay(1);
    }
    return true;
}

// ------------------------------------------------------------------- program
bool fpga_program_from_file(const char *path) {
    File f = LittleFS.open(path, "r");
    if (!f) {
        Serial.printf("# bitstream not found: %s\n", path);
        return false;
    }

    Serial.printf("# programming FPGA from %s (%u bytes)...\n",
                  path, (unsigned)f.size());

    // 1. Hold the device in configuration reset, then release and let it
    //    initialise. RESET_N is the board's program/init control.
    pinMode(PIN_FPGA_RESET_N, OUTPUT);
    digitalWrite(PIN_FPGA_RESET_N, LOW);
    delay(10);

    pinMode(PIN_FPGA_CDONE, INPUT);
    digitalWrite(PIN_FPGA_CS_N, HIGH);      // idle chip select

    // configure SPI for the configuration transfer
    SPI.begin();
    SPI.setSCK(PIN_FPGA_SCK);
    SPI.setTX(PIN_FPGA_MOSI);
    SPI.setRX(PIN_FPGA_MISO);

    // 2. Release reset; the device clears CDONE and samples its mode pins.
    digitalWrite(PIN_FPGA_RESET_N, HIGH);
    delay(5);
    if (digitalRead(PIN_FPGA_CDONE) == HIGH) {
        // already configured and not held in reset - pulse reset first
        digitalWrite(PIN_FPGA_RESET_N, LOW);
        delay(10);
        digitalWrite(PIN_FPGA_RESET_N, HIGH);
        delay(5);
    }

    // 3. Slave-SPI configuration: assert CS, stream the bitstream MSB first.
    //    The bitstream file is the raw .bin from Diamond's Deployment tool.
    digitalWrite(PIN_FPGA_CS_N, LOW);
    delayMicroseconds(50);

    uint8_t buf[256];
    while (f.available()) {
        size_t n = f.read(buf, sizeof(buf));
        for (size_t i = 0; i < n; i++)
            SPI.transfer(buf[i]);
    }
    f.close();

    // 4. Deassert CS and wait for the device to report configuration done.
    digitalWrite(PIN_FPGA_CS_N, HIGH);

    bool ok = wait_cdone(true, 2000);
    Serial.printf("# FPGA configuration %s (CDONE=%d)\n",
                  ok ? "OK" : "FAILED", digitalRead(PIN_FPGA_CDONE));
    return ok;
}

bool fpga_select_mode(int mode) {
    const char *path = (mode == 1) ? BITSTREAM_FFT : BITSTREAM_RAW;
    return fpga_program_from_file(path);
}
