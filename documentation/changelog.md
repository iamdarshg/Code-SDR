# Changelog

## [Unreleased]

### Added
- Created this changelog to track development progress.
- **Verilator Test Runner:** Introduced `tests/scripts/run_all_tests.sh` to handle complex test orchestration directly within WSL. This replaces the problematic `run_all_tests.bat` as the primary test runner for Verilator and cocotb simulations.

### Changed
- **Verilator Test Platform:**
  - Modified `tests/scripts/run_all_tests.bat` to improve performance and stability.
  - Removed the creation of temporary directories and file copying between Windows and WSL for each test run. Tests now execute directly from the project's WSL-mounted path.
  - Made the Python virtual environment setup idempotent. The script now checks if the environment exists and installs dependencies only if necessary, significantly speeding up subsequent test runs.
  - Standardized output directory paths for better consistency.
  - Resolved `cmd.exe` parsing issues in `run_all_tests.bat` by replacing complex conditional logic with a single call to a new WSL shell script (`run_all_tests.sh`).
- **Verilog Modules:**
  - `nco_generator.v`: Replaced procedural ROM initialization with hardcoded values and fixed negative number representation for Verilator compilation.
  - `async_fifo_tb.v`: Changed non-blocking assignments (`<=`) to blocking assignments (`=`) in `initial` blocks to resolve Verilator compilation errors.
  - `clock_manager.v`: Enhanced with behavioral model for multi-domain clock generation (105MHz to 1.2GHz) and reset synchronization to support integration testing.

- **Integration & Testing (Issue 11):**
  - **New Tests**: Implemented `tests/cocotb_tests/test_fpga_processing_pipeline.py` for end-to-end data flow validation.
  - **Performance**: Executed and fixed `tests/test_fft_ddc_performance.py` benchmark suite; achieved 51.1 MSPS FFT throughput.
  - **Software Fixes**: Optimized DDC filter for complex signals and added fallback for Remez filter design convergence issues.
  - **Documentation**: Generated `ISSUE_11_VERIFICATION_REPORT.md` summarizing integration and performance results.
