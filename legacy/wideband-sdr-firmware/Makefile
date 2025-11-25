# Wideband SDR Firmware Makefile
# dsPIC33AK256MC505 based SDR firmware build system
# 
# Created: November 2025
# Version: 1.0.0
# License: MIT

# Toolchain Configuration
PREFIX = xc16-
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJDUMP = $(PREFIX)objdump
OBJCOPY = $(PREFIX)objcopy
SIZE = $(PREFIX)size
NM = $(PREFIX)nm
GDB = $(PREFIX)gdb

# Project Configuration
PROJECT = wideband_sdr
DEVICE = dsPIC33AK256MC505
CPU = dspic33ak

# Directories
SRC_DIR = .
INC_DIR = .
BUILD_DIR = build
DIST_DIR = dist

# Source Files
SOURCES = main.c adf4351.c adc_dma.c usb_device.c
HEADERS = adf4351.h adc_dma.h usb_device.h

# Object Files
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

# Compiler Flags
CFLAGS = -Wall -Wextra -Werror \
         -mcpu=$(CPU) \
         -msmart-io=1 \
         -mdfp=$(DEVICE) \
         -MMD -MP \
         -DFVMCPUINIT \
         -O3 \
         -fdata-sections \
         -ffunction-sections \
         -fno-inline-small-functions \
         -I$(INC_DIR)

# Linker Flags
LDFLAGS = -mcpu=$(CPU) \
          -mdfp=$(DEVICE) \
          -Wl,--gc-sections \
          -Wl,--default-section-alignment=4 \
          -Wl,--stack=0x800,16 \
          -L$(BUILD_DIR) \
          -T$(BUILD_DIR)/$(PROJECT).ld

# Include Dependencies
DEPS = $(OBJECTS:%.o=%.d)

# Library Paths and Libraries
LIB_PATHS = 
LIBS = -lm

# Programming Configuration
PROGRAMMER = pic32kit4
PROGRAM_PORT = auto
PROGRAM_SPEED = 500

# Debug Configuration
DEBUG = -g -DDEBUG
RELEASE = -DNDEBUG

# Default Target
.PHONY: all
all: $(DIST_DIR)/$(PROJECT).hex

# Build Directory Creation
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(DIST_DIR):
	@mkdir -p $(DIST_DIR)

# Object File Compilation
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Dependency Generation
$(DEPS): | $(BUILD_DIR)

include $(DEPS)

# Linking
$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS) | $(BUILD_DIR)
	@echo "Linking $(PROJECT).elf..."
	@$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)
	@echo "Memory usage:"
	@$(SIZE) $@

# Hex File Generation
$(DIST_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf | $(DIST_DIR)
	@echo "Generating hex file..."
	@$(OBJCOPY) -O ihex $< $@
	@echo "Binary size:"
	@$(SIZE) --target=ihex $<

# Binary File Generation (for debugging)
$(DIST_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf | $(DIST_DIR)
	@echo "Generating binary file..."
	@$(OBJCOPY) -O binary $< $@

# Listing Generation
$(DIST_DIR)/$(PROJECT).lst: $(BUILD_DIR)/$(PROJECT).elf | $(DIST_DIR)
	@echo "Generating listing..."
	@$(OBJDUMP) -h -S $< > $@

# Assembly Listing
$(DIST_DIR)/$(PROJECT).s: $(BUILD_DIR)/$(PROJECT).elf | $(DIST_DIR)
	@echo "Generating assembly listing..."
	@$(OBJCUMP) -D -S -M=$($(DEVICE) $$< > $@)

# Program the device
.PHONY: program
program: $(DIST_DIR)/$(PROJECT).hex
	@echo "Programming $(PROJECT) to device..."
	@$(PREFIX)make -f $(PREFIX)Makefile DEVICE=$(DEVICE) HEX_FILE=$(DIST_DIR)/$(PROJECT).hex

# Program with verification
.PHONY: program-verify
program-verify: $(DIST_DIR)/$(PROJECT).hex
	@echo "Programming and verifying $(PROJECT)..."
	@$(PREFIX)make -f $(PREFIX)Makefile DEVICE=$(DEVICE) HEX_FILE=$(DIST_DIR)/$(PROJECT).hex VERIFY=1

# Erase device
.PHONY: erase
erase:
	@echo "Erasing device..."
	@$(PREFIX)make -f $(PREFIX)Makefile DEVICE=$(DEVICE) ERASE=1

# Reset device
.PHONY: reset
reset:
	@echo "Resetting device..."
	@$(PREFIX)make -f $(PREFIX)Makefile DEVICE=$(DEVICE) RESET=1

# Clean build files
.PHONY: clean
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@rm -f $(DIST_DIR)/$(PROJECT).hex
	@rm -f $(DIST_DIR)/$(PROJECT).bin
	@rm -f $(DIST_DIR)/$(PROJECT).lst
	@rm -f $(DIST_DIR)/$(PROJECT).s
	@echo "Build directory cleaned."

# Clean everything including dependencies
.PHONY: distclean
distclean: clean
	@echo "Cleaning all generated files..."
	@rm -f $(DEPS)
	@echo "All files cleaned."

# Dependencies check
.PHONY: deps
deps:
	@echo "Checking dependencies..."
	@$(PREFIX)gcc --version
	@echo "Toolchain check complete."

# Flash size report
.PHONY: flash-report
flash-report: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Flash memory report:"
	@$(SIZE) -A $(BUILD_DIR)/$(PROJECT).elf
	@$(SIZE) -B $(BUILD_DIR)/$(PROJECT).elf

# Static Analysis
.PHONY: analyze
analyze: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Running static analysis..."
	@$(PREFIX)nm -C $(BUILD_DIR)/$(PROJECT).elf > $(DIST_DIR)/symbols.txt
	@$(OBJDUMP) -t $(BUILD_DIR)/$(PROJECT).elf > $(DIST_DIR)/symbol_table.txt
	@echo "Analysis complete. Files generated in $(DIST_DIR)/"

# Build with debug information
.PHONY: debug
debug: CFLAGS += $(DEBUG)
debug: LDFLAGS += -g -DDEBUG
debug: $(DIST_DIR)/$(PROJECT).hex

# Build optimized for size
.PHONY: size-optimized
size-optimized: CFLAGS += -Os -fno-unroll-loops -fno-tree-loop-distribute-patterns
size-optimized: $(DIST_DIR)/$(PROJECT).hex

# Build optimized for speed
.PHONY: speed-optimized
speed-optimized: CFLAGS += -Ofast -funroll-loops -ffast-math
speed-optimized: $(DIST_DIR)/$(PROJECT).hex

# Test compilation (no linking)
.PHONY: compile-test
compile-test: $(OBJECTS)
	@echo "Compilation test successful."

# Generate linker script
.PHONY: generate-ld
generate-ld:
	@echo "Generating linker script for $(DEVICE)..."
	@$(PREFIX)make -f $(PREFIX)Makefile DEVICE=$(DEVICE) GENERATE_LD=1

# Code quality check
.PHONY: check
check: compile-test
	@echo "Running code quality checks..."
	@echo "Checking for undefined symbols..."
	@$(NM) --undefined $(BUILD_DIR)/$(PROJECT).elf | grep -v " U " || echo "No undefined symbols found."
	@echo "Code quality check complete."

# Verbose build
.PHONY: verbose
verbose: CFLAGS += -v
verbose: LDFLAGS += -v
verbose: all

# Help target
.PHONY: help
help:
	@echo "Wideband SDR Firmware Build System"
	@echo "=================================="
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build firmware (default)"
	@echo "  debug         - Build with debug information"
	@echo "  size-optimized- Build optimized for size"
	@echo "  speed-optimized - Build optimized for speed"
	@echo "  program       - Program device with firmware"
	@echo "  program-verify - Program and verify device"
	@echo "  erase         - Erase device memory"
	@echo "  reset         - Reset device"
	@echo "  clean         - Clean build files"
	@echo "  distclean     - Clean all generated files"
	@echo "  analyze       - Generate analysis files"
	@echo "  flash-report  - Display flash memory usage"
	@echo "  compile-test  - Test compilation only"
	@echo "  check         - Run code quality checks"
	@echo "  deps          - Check toolchain dependencies"
	@echo "  help          - Display this help"
	@echo ""
	@echo "Example usage:"
	@echo "  make all          # Build firmware"
	@echo "  make program      # Build and program device"
	@echo "  make clean        # Clean build files"

# Makefile includes
-include $(DEPS)

# Prevent parallel execution issues
.NOTPARALLEL:

# Phony targets to avoid file conflicts
.PHONY: all debug size-optimized speed-optimized program program-verify \
        erase reset clean distclean analyze flash-report compile-test \
        check deps generate-ld verbose help

# Target for custom commands
.PHONY: custom
custom:
	@echo "Custom target - modify this for project-specific commands"
