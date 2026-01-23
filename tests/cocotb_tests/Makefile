# Makefile for cocotb tests

# Defaults
SIM ?= icarus
TOPLEVEL_LANG ?= verilog

# Point to the Verilog source files
VERILOG_SOURCES = $(shell find ../../verilog -name "*.v")

# TOPLEVEL is the name of the toplevel module in your Verilog design
TOPLEVEL ?= cic_decimator

# MODULE is the basename of the Python test file
MODULE ?= test_cic_decimator

# Include cocotb's make rules
include $(shell cocotb-config --makefiles)/Makefile.sim
