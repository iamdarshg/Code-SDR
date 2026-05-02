# Makefile for cocotb tests

SIM ?= verilator
TOPLEVEL_LANG ?= verilog

# Common sources
VERILOG_SOURCES += $(CURDIR)/../../verilog/adc_interface.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/nco_generator.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/cic_decimator.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/async_fifo.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/fft_processor.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/hamming_window.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/ethernet_mac.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/udp_ip_stack.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/rp2040_interface.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/digital_downconverter.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/compensation_filter.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/dma_engine.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/adaptive_gain_scaler.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/average_power_detector.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/clock_manager.v
VERILOG_SOURCES += $(CURDIR)/../../verilog/fpga_processing_pipeline.v

# Use EXTRA_ARGS for verilator
ifeq ($(SIM),verilator)
    EXTRA_ARGS += --timing
    EXTRA_ARGS += -DSIMULATION
    # Verilator specific flags to ignore width expansion warnings often seen in DSP code
    EXTRA_ARGS += -Wno-WIDTHEXPAND
endif

include $(shell cocotb-config --makefiles)/Makefile.sim
