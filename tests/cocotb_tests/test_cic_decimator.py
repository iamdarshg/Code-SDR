import cocotb
from cocotb.triggers import Timer, RisingEdge
from cocotb.clock import Clock
import numpy as np

async def reset_dut(dut):
    dut.rst_n.value = 0
    await Timer(10, units='ns')
    dut.rst_n.value = 1
    await Timer(10, units='ns')

@cocotb.test()
async def test_cic_decimator(dut):
    # Clock
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset
    await reset_dut(dut)

    # Test parameters
    input_width = dut.INPUT_WIDTH.value
    stages = dut.STAGES.value
    decimation = dut.DECIMATION.value

    # Generate input signal (a simple ramp)
    test_data = np.arange(0, 256, 1, dtype=np.int32)

    # Golden model
    integrator = np.zeros(stages, dtype=np.int64)
    comb = np.zeros(stages, dtype=np.int64)
    comb_delay = np.zeros(stages, dtype=np.int64)
    expected_output = []

    for i in range(len(test_data)):
        # Integrator stage
        integrator[0] += test_data[i]
        for j in range(1, stages):
            integrator[j] += integrator[j-1]

        # Decimator and Comb stage
        if (i + 1) % decimation == 0:
            temp_in = integrator[stages-1]
            comb[0] = temp_in - comb_delay[0]
            comb_delay[0] = temp_in
            for j in range(1, stages):
                temp_in = comb[j-1]
                comb[j] = temp_in - comb_delay[j]
                comb_delay[j] = temp_in

            # Gain compensation
            gain = decimation ** stages
            output = comb[stages-1] // gain
            expected_output.append(output)

    # Drive DUT
    output_from_dut = []
    for data in test_data:
        dut.data_in.value = int(data)
        dut.data_valid.value = 1
        await RisingEdge(dut.clk)
        if dut.output_valid.value == 1:
            output_from_dut.append(dut.data_out.value.signed_integer)

    dut.data_valid.value = 0

    # Compare results
    assert len(output_from_dut) == len(expected_output), f"Output length mismatch: DUT={len(output_from_dut)}, Expected={len(expected_output)}"
    for i in range(len(expected_output)):
        assert output_from_dut[i] == expected_output[i], f"Mismatch at index {i}: DUT={output_from_dut[i]}, Expected={expected_output[i]}"

