#!/bin/bash
# run_all_tests.sh - Code-SDR FPGA Test Runner (WSL-native)
# Tests all modules using cocotb and/or Verilator
# Designed to be run directly from WSL or called by a Windows .bat script.

set -e # Exit on first error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PROJECT_ROOT=$(pwd) # Assumes script is run from project root or cd'd into it
VERILOG_DIR="${PROJECT_ROOT}/verilog"
TEST_DIR="${PROJECT_ROOT}/tests"
OUTPUT_DIR="${PROJECT_ROOT}/tests/sim_output" # Ensure absolute path for output

mkdir -p "$OUTPUT_DIR"

echo -e "${YELLOW}========================================${NC}"
echo -e "${YELLOW}  SDR FPGA Virtual Simulation Suite    ${NC}"
echo -e "${YELLOW}========================================${NC}"

# Set up Python virtual environment idempotently
echo -e "${YELLOW}Setting up Python virtual environment...${NC}"
cd "$TEST_DIR"
# Remove and recreate venv to ensure a clean state, then install dependencies
if [ -d "venv" ]; then
    echo "  Removing existing venv..."
    rm -rf venv
fi
echo "  Creating new venv and installing requirements..."
/usr/bin/python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
cd "$PROJECT_ROOT"

# Set Python environment variables for cocotb
export COCOTB_REDUCED_LOG_FMT=1
export COCOTB_SIM=verilator
export TOPLEVEL_LANG=verilog

# Initialize counters
PASSED=0
FAILED=0
TIMEOUTS=0
TOTAL_MODULES=0

# Test timeout in seconds
SIM_TIMEOUT=10

# Module list (all with available testbenches)
MODULES=(
    "adc_interface"
    "nco_generator"
    "cic_decimator"
    "async_fifo"
    "ethernet_mac"
    "fft_processor"
    "hamming_window"
    "rp2040_interface"
    "udp_ip_stack"
    "adaptive_gain_scaler"
    "average_power_detector"
    "digital_downconverter"
    "dma_engine"
)
TOTAL_MODULES=${#MODULES[@]}

echo ""
echo "Testing ${MODULES[@]}"
echo "Timeout: ${SIM_TIMEOUT} seconds"
echo ""

for module in "${MODULES[@]}"; do
    set -x # Enable debugging
    echo -e "\n${YELLOW}=======================================${NC}"
    echo -e "${YELLOW}Testing Module: ${module}${NC}"
    echo -e "${YELLOW}=======================================${NC}"

    TEST_RESULT="UNKNOWN"

    # Cocotb Test
    COCOTB_TEST_FILE="${TEST_DIR}/cocotb_tests/test_${module}.py"
    if [ -f "$COCOTB_TEST_FILE" ]; then
        echo "Running cocotb test..."
        export TOPLEVEL="$module"
        export MODULE="test_${module}"
        export COCOTB_OUT_DIR="${OUTPUT_DIR}/${module}_out"
        export SIM_BUILD="${OUTPUT_DIR}/${module}_sim_build"
        
        mkdir -p "$COCOTB_OUT_DIR"
        mkdir -p "$SIM_BUILD"

        COCOTB_LOG_FILE="${OUTPUT_DIR}/${module}_cocotb.log"
        cd "$TEST_DIR"
        source venv/bin/activate
        python -m pytest -p pytest_cocotb --sim=verilator --toplevel="$module" "cocotb_tests/test_${module}.py" -v --tb=no > "$COCOTB_LOG_FILE" # Removed 2>&1 for debugging
        COCOTB_EXIT_CODE=$?
        cd "$PROJECT_ROOT"

        if [ "$COCOTB_EXIT_CODE" -eq 0 ]; then
            echo -e "${GREEN}  cocotb: PASS${NC}"
            TEST_RESULT="PASS"
        else
            echo -e "${RED}  cocotb: FAIL${NC}"
            TEST_RESULT="FAIL"
        fi
    else
        echo "  cocotb: NOT AVAILABLE"
    fi

    # Verilator Test (only if no cocotb test available, or if cocotb test failed and Verilator is needed for compile check)
    VERILOG_MODULE_FILE="${VERILOG_DIR}/${module}.v"
    VERILOG_TESTBENCH_FILE="${VERILOG_DIR}/${module}_tb.v"

    if [ -f "$VERILOG_MODULE_FILE" ] && [ -f "$VERILOG_TESTBENCH_FILE" ]; then
        if [ "$TEST_RESULT" = "UNKNOWN" ] || [ "$TEST_RESULT" = "FAIL" ]; then
            echo "Running verilator test..."

            VERILATOR_BUILD_DIR="${OUTPUT_DIR}/${module}_verilator_build"
            mkdir -p "$VERILATOR_BUILD_DIR"

            # Compile with Verilator
            COMP_LOG_FILE="${OUTPUT_DIR}/${module}_compile.log"
            verilator --cc --exe "${VERILOG_TESTBENCH_FILE}" "${VERILOG_MODULE_FILE}" --top-module "${module}_tb" --Mdir "${VERILATOR_BUILD_DIR}" --timing --Wno-WIDTHEXPAND > "$COMP_LOG_FILE" # Removed 2>&1 for debugging
            VERILATOR_COMP_EXIT_CODE=$?

            if [ "$VERILATOR_COMP_EXIT_CODE" -eq 0 ]; then
                echo -e "${GREEN}  compilation: OK${NC}"
                # Build Verilator executable
                BUILD_LOG_FILE="${OUTPUT_DIR}/${module}_build.log"
                make -C "${VERILATOR_BUILD_DIR}" -f "V${module}_tb.mk" > "$BUILD_LOG_FILE" # Removed 2>&1 for debugging
                VERILATOR_BUILD_EXIT_CODE=$?

                if [ "$VERILATOR_BUILD_EXIT_CODE" -eq 0 ]; then
                    echo -e "${GREEN}  build: OK${NC}"
                    # Run with timeout
                    SIM_LOG_FILE="${OUTPUT_DIR}/${module}_sim.log"
                    timeout "$SIM_TIMEOUT"s "${VERILATOR_BUILD_DIR}/V${module}_tb" > "$SIM_LOG_FILE" # Removed 2>&1 for debugging
                    VERILATOR_SIM_EXIT_CODE=$?
                    
                    # Check for test pass/fail in output (if the testbench uses $display("TEST PASSED"))
                    if grep -q "TEST PASSED" "$SIM_LOG_FILE"; then
                        echo -e "${GREEN}  verilator: PASS${NC}"
                        if [ "$TEST_RESULT" = "UNKNOWN" ]; then
                            TEST_RESULT="PASS"
                        fi
                    elif [ "$VERILATOR_SIM_EXIT_CODE" -eq 124 ]; then # timeout command exit code for timeout
                        echo -e "${RED}  verilator: TIMEOUT${NC}"
                        TEST_RESULT="FAIL"
                        TIMEOUTS=$((TIMEOUTS + 1))
                    else
                        echo -e "${RED}  verilator: SIMULATION FAILED${NC}"
                        TEST_RESULT="FAIL"
                    fi
                else
                    echo -e "${RED}  verilator: BUILD FAIL${NC}"
                    TEST_RESULT="FAIL"
                fi
            else
                echo -e "${RED}  verilator: COMPILE FAIL${NC}"
                TEST_RESULT="FAIL"
            fi
        fi # End if [ "$TEST_RESULT" = "UNKNOWN" ] || [ "$TEST_RESULT" = "FAIL" ];
    else
        echo "  verilator: NOT AVAILABLE (missing .v or _tb.v)"
    fi

    # Count results
    if [ "$TEST_RESULT" = "PASS" ]; then
        PASSED=$((PASSED + 1))
    elif [ "$TEST_RESULT" = "FAIL" ]; then
        FAILED=$((FAILED + 1))
    fi
    set +x # Disable debugging
done

echo -e "\n${YELLOW}========================================${NC}"
echo -e "Results: ${GREEN}${PASSED} passed${NC}, ${RED}${FAILED} failed${NC}"
echo -e "${YELLOW}========================================${NC}"

# Clean up temporary exit code files
rm -f "${OUTPUT_DIR}/cocotb_exit_code.tmp"
rm -f "${OUTPUT_DIR}/verilator_comp_exit_code.tmp"
rm -f "${OUTPUT_DIR}/verilator_build_exit_code.tmp"
rm -f "${OUTPUT_DIR}/verilator_sim_exit_code.tmp"

# Exit with error if any tests failed
if [ "$FAILED" -ne 0 ] || [ "$TIMEOUTS" -ne 0 ]; then
    exit 1
fi
