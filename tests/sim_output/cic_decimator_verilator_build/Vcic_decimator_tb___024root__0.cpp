// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vcic_decimator_tb.h for the primary calling header

#include "Vcic_decimator_tb__pch.h"

VlCoroutine Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__0(Vcic_decimator_tb___024root* vlSelf);
VlCoroutine Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__1(Vcic_decimator_tb___024root* vlSelf);
VlCoroutine Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__2(Vcic_decimator_tb___024root* vlSelf);

void Vcic_decimator_tb___024root___eval_initial(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_initial\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
    Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__2(vlSelf);
}

VlCoroutine Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__0(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.cic_decimator_tb__DOT__clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "verilog/cic_decimator_tb.v", 
                                             38);
        vlSelfRef.cic_decimator_tb__DOT__clk = (1U 
                                                & (~ (IData)(vlSelfRef.cic_decimator_tb__DOT__clk)));
    }
}

VlCoroutine Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__1(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x0000e35fa931a000ULL, 
                                         nullptr, "verilog/cic_decimator_tb.v", 
                                         47);
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.cic_decimator_tb__DOT__test_completed)))))) {
        VL_WRITEF_NX("TIMEOUT: Test did not complete within 250 seconds\nTEST FAILED: Timeout occurred during CIC decimator testing\n[CIC_DECIMATOR_TB] Dumping timeout state:\n[CIC_DECIMATOR_TB]   Simulation time: %0t ns\n[CIC_DECIMATOR_TB]   rst_n=%b, data_valid=%b\n[CIC_DECIMATOR_TB]   data_in=%x, data_out=%x\n[CIC_DECIMATOR_TB]   output_valid=%b\n[CIC_DECIMATOR_TB]   Loop counter i=%0d\n[CIC_DECIMATOR_TB]   Test state: test_passed=%b\n[CIC_DECIMATOR_TB]   CIC internal state dump:\n",0,
                     64,VL_TIME_UNITED_Q(1000),-9,1,
                     (IData)(vlSelfRef.cic_decimator_tb__DOT__rst_n),
                     1,vlSelfRef.cic_decimator_tb__DOT__data_valid,
                     32,vlSelfRef.cic_decimator_tb__DOT__data_in,
                     32,vlSelfRef.cic_decimator_tb__DOT__dut__DOT__output_register,
                     1,(IData)(vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable),
                     32,vlSelfRef.cic_decimator_tb__DOT__i,
                     1,(IData)(vlSelfRef.cic_decimator_tb__DOT__test_passed));
        VL_FINISH_MT("verilog/cic_decimator_tb.v", 61, "");
    }
}

VlCoroutine Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__2(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_initial__TOP__Vtiming__2\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    VL_WRITEF_NX("[CIC_DECIMATOR_TB] Starting CIC Decimator testbench simulation\n[CIC_DECIMATOR_TB] Parameters: INPUT_WIDTH=32, OUTPUT_WIDTH=32, STAGES=3, DECIMATION=8\n[CIC_DECIMATOR_TB] Clock: 100 MHz (period 10 ns)\n",0);
    vlSelfRef.cic_decimator_tb__DOT__rst_n = 1U;
    vlSelfRef.cic_decimator_tb__DOT__data_in = 0U;
    vlSelfRef.cic_decimator_tb__DOT__data_valid = 0U;
    vlSelfRef.cic_decimator_tb__DOT__test_passed = 1U;
    VL_WRITEF_NX("[CIC_DECIMATOR_TB] Applying reset...\n",0);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/cic_decimator_tb.v", 
                                         78);
    vlSelfRef.cic_decimator_tb__DOT__rst_n = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/cic_decimator_tb.v", 
                                         80);
    vlSelfRef.cic_decimator_tb__DOT__rst_n = 1U;
    VL_WRITEF_NX("[CIC_DECIMATOR_TB] Reset complete. Waiting for stabilization...\n",0);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/cic_decimator_tb.v", 
                                         86);
    VL_WRITEF_NX("[CIC_DECIMATOR_TB] Starting impulse response test...\n",0);
    vlSelfRef.cic_decimator_tb__DOT__data_in = 0x00010000U;
    vlSelfRef.cic_decimator_tb__DOT__data_valid = 1U;
    VL_WRITEF_NX("[CIC_DECIMATOR_TB] Sending impulse: data_in=%x, data_valid=%b\n",0,
                 32,vlSelfRef.cic_decimator_tb__DOT__data_in,
                 1,(IData)(vlSelfRef.cic_decimator_tb__DOT__data_valid));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "verilog/cic_decimator_tb.v", 
                                         94);
    vlSelfRef.cic_decimator_tb__DOT__data_valid = 0U;
    VL_WRITEF_NX("[CIC_DECIMATOR_TB] Feeding 7 zeros...\n",0);
    vlSelfRef.cic_decimator_tb__DOT__i = 0U;
    while (VL_GTS_III(32, 7U, vlSelfRef.cic_decimator_tb__DOT__i)) {
        vlSelfRef.cic_decimator_tb__DOT__data_in = 0U;
        vlSelfRef.cic_decimator_tb__DOT__data_valid = 1U;
        VL_WRITEF_NX("[CIC_DECIMATOR_TB]   Sample %0d: data_in=%x, data_valid=%b\n",0,
                     32,vlSelfRef.cic_decimator_tb__DOT__i,
                     32,vlSelfRef.cic_decimator_tb__DOT__data_in,
                     1,(IData)(vlSelfRef.cic_decimator_tb__DOT__data_valid));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                             nullptr, 
                                             "verilog/cic_decimator_tb.v", 
                                             103);
        vlSelfRef.cic_decimator_tb__DOT__i = ((IData)(1U) 
                                              + vlSelfRef.cic_decimator_tb__DOT__i);
    }
    vlSelfRef.cic_decimator_tb__DOT__data_valid = 0U;
    VL_WRITEF_NX("[CIC_DECIMATOR_TB] Input sequence complete. Waiting for output...\n[CIC_DECIMATOR_TB] Waiting for output_valid...\n",0);
    while ((1U & (~ (IData)(vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable)))) {
        co_await vlSelfRef.__VtrigSched_h4b095651__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( cic_decimator_tb.dut.decimate_enable)", 
                                                             "verilog/cic_decimator_tb.v", 
                                                             111);
    }
    VL_WRITEF_NX("[CIC_DECIMATOR_TB] Output received: data_out=%x\n[CIC_DECIMATOR_TB] Impulse response: input=0x00010000, output=0x%x\n",0,
                 32,vlSelfRef.cic_decimator_tb__DOT__dut__DOT__output_register,
                 32,vlSelfRef.cic_decimator_tb__DOT__dut__DOT__output_register);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000f4240ULL, 
                                         nullptr, "verilog/cic_decimator_tb.v", 
                                         116);
    VL_WRITEF_NX("[CIC_DECIMATOR_TB] CIC Decimator basic test completed\n[CIC_DECIMATOR_TB] Analyzing output...\n[CIC_DECIMATOR_TB]   Output appears valid\nTEST PASSED\n[CIC_DECIMATOR_TB] Dumping final testbench state:\n[CIC_DECIMATOR_TB]   rst_n=%b, data_valid=%b\n[CIC_DECIMATOR_TB]   data_in=%x, data_out=%x\n[CIC_DECIMATOR_TB]   output_valid=%b\n[CIC_DECIMATOR_TB]   Loop counter i=%0d\n[CIC_DECIMATOR_TB]   Test result: %s\n",0,
                 1,vlSelfRef.cic_decimator_tb__DOT__rst_n,
                 1,(IData)(vlSelfRef.cic_decimator_tb__DOT__data_valid),
                 32,vlSelfRef.cic_decimator_tb__DOT__data_in,
                 32,vlSelfRef.cic_decimator_tb__DOT__dut__DOT__output_register,
                 1,(IData)(vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable),
                 32,vlSelfRef.cic_decimator_tb__DOT__i,
                 48,((IData)(vlSelfRef.cic_decimator_tb__DOT__test_passed)
                      ? 0x0000504153534544ULL : 0x00004641494c4544ULL));
    vlSelfRef.cic_decimator_tb__DOT__test_completed = 1U;
    VL_FINISH_MT("verilog/cic_decimator_tb.v", 140, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vcic_decimator_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vcic_decimator_tb___024root___eval_triggers__act(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_triggers__act\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((((IData)(vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable) 
                                                        != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__dut__DOT__decimate_enable__0)) 
                                                       << 3U) 
                                                      | (vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                         << 2U)) 
                                                     | ((((~ (IData)(vlSelfRef.cic_decimator_tb__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.cic_decimator_tb__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__clk__0 
        = vlSelfRef.cic_decimator_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__rst_n__0 
        = vlSelfRef.cic_decimator_tb__DOT__rst_n;
    vlSelfRef.__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__dut__DOT__decimate_enable__0 
        = vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable;
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.__VactDidInit)))))) {
        vlSelfRef.__VactDidInit = 1U;
        vlSelfRef.__VactTriggered[0U] = (8ULL | vlSelfRef.__VactTriggered
                                         [0U]);
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vcic_decimator_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vcic_decimator_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vcic_decimator_tb___024root___nba_sequent__TOP__0(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___nba_sequent__TOP__0\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    QData/*40:0*/ __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v0;
    __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v0 = 0;
    CData/*0:0*/ __VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v0;
    __VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v0 = 0;
    QData/*40:0*/ __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v1;
    __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v1 = 0;
    QData/*40:0*/ __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v4;
    __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v4 = 0;
    CData/*0:0*/ __VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v4;
    __VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v4 = 0;
    QData/*40:0*/ __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v5;
    __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v5 = 0;
    QData/*40:0*/ __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v8;
    __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v8 = 0;
    CData/*0:0*/ __VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v8;
    __VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v8 = 0;
    QData/*40:0*/ __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v9;
    __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v9 = 0;
    // Body
    vlSelfRef.__Vdly__cic_decimator_tb__DOT__dut__DOT__decimate_counter 
        = vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_counter;
    vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v0 = 0U;
    vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v1 = 0U;
    vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v2 = 0U;
    vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v3 = 0U;
    vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v4 = 0U;
    vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v5 = 0U;
    if (vlSelfRef.cic_decimator_tb__DOT__rst_n) {
        if (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable) {
            vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j = 1U;
            while (VL_LTS_III(32, 0U, vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j)) {
                __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v0 
                    = vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_delay
                    [0U][(1U & (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j 
                                - (IData)(1U)))];
                __VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v0 
                    = (1U & vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j);
                vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v0, 0U, (IData)(__VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v0));
                vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j 
                    = ((IData)(1U) + vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j);
            }
            __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v1 
                = vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb
                [0U];
            vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v1, 0U, 0U);
        }
    } else {
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j = 2U;
        vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(0ULL, 0U, 0U);
        vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(0ULL, 0U, 1U);
    }
    if (vlSelfRef.cic_decimator_tb__DOT__rst_n) {
        if (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable) {
            vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j = 1U;
            while (VL_LTS_III(32, 0U, vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j)) {
                __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v4 
                    = vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_delay
                    [1U][(1U & (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j 
                                - (IData)(1U)))];
                __VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v4 
                    = (1U & vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j);
                vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v4, 1U, (IData)(__VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v4));
                vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j 
                    = ((IData)(1U) + vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j);
            }
            __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v5 
                = vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb
                [1U];
            vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v5, 1U, 0U);
        }
    } else {
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j = 2U;
        vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(0ULL, 1U, 0U);
        vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(0ULL, 1U, 1U);
    }
    if (vlSelfRef.cic_decimator_tb__DOT__rst_n) {
        if (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable) {
            vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j = 1U;
            while (VL_LTS_III(32, 0U, vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j)) {
                __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v8 
                    = vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_delay
                    [2U][(1U & (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j 
                                - (IData)(1U)))];
                __VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v8 
                    = (1U & vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j);
                vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v8, 2U, (IData)(__VdlyDim0__cic_decimator_tb__DOT__dut__DOT__comb_delay__v8));
                vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j 
                    = ((IData)(1U) + vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j);
            }
            __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v9 
                = vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb
                [2U];
            vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb_delay__v9, 2U, 0U);
        }
    } else {
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_loop_j = 2U;
        vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(0ULL, 2U, 0U);
        vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.enqueue(0ULL, 2U, 1U);
    }
    if (vlSelfRef.cic_decimator_tb__DOT__rst_n) {
        if (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable) {
            vlSelfRef.__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v0 
                = (0x000001ffffffffffULL & (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_delay
                                            [0U][0U] 
                                            - vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_delay
                                            [0U][1U]));
            vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v0 = 1U;
            vlSelfRef.__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v2 
                = (0x000001ffffffffffULL & (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_delay
                                            [1U][0U] 
                                            - vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_delay
                                            [1U][1U]));
            vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v2 = 1U;
            vlSelfRef.__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v4 
                = (0x000001ffffffffffULL & (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_delay
                                            [2U][0U] 
                                            - vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_delay
                                            [2U][1U]));
            vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v4 = 1U;
        }
    } else {
        vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v1 = 1U;
        vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v3 = 1U;
        vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v5 = 1U;
    }
    vlSelfRef.__VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay.commit(vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb_delay);
}

void Vcic_decimator_tb___024root___nba_sequent__TOP__1(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___nba_sequent__TOP__1\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY((vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable))) {
        VL_WRITEF_NX("Time=%0t: Data out=0x%x\n",0,
                     64,VL_TIME_UNITED_Q(1000),-9,32,
                     vlSelfRef.cic_decimator_tb__DOT__dut__DOT__output_register);
    }
}

void Vcic_decimator_tb___024root___nba_sequent__TOP__2(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___nba_sequent__TOP__2\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.cic_decimator_tb__DOT__rst_n) {
        if (vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable) {
            vlSelfRef.cic_decimator_tb__DOT__dut__DOT__output_register 
                = ((1U & (IData)((1ULL & (VL_SHIFTR_QQI(41,41,32, 
                                                        (0x000001ffffffffffULL 
                                                         & (0x0000000000000100ULL 
                                                            + 
                                                            vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb
                                                            [2U])), 9U) 
                                          >> 0x28U))))
                    ? 0x80000000U : ((0U != (0x000001ffU 
                                             & (IData)(
                                                       (0x00000000000001ffULL 
                                                        & (VL_SHIFTR_QQI(41,41,32, 
                                                                         (0x000001ffffffffffULL 
                                                                          & (0x0000000000000100ULL 
                                                                             + 
                                                                             vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb
                                                                             [2U])), 9U) 
                                                           >> 0x20U)))))
                                      ? 0x7fffffffU
                                      : (IData)((0x00000000ffffffffULL 
                                                 & ((0x0000000000000100ULL 
                                                     + 
                                                     vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb
                                                     [2U]) 
                                                    >> 9U)))));
        }
        if (vlSelfRef.cic_decimator_tb__DOT__data_valid) {
            if ((7U == (IData)(vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_counter))) {
                vlSelfRef.__Vdly__cic_decimator_tb__DOT__dut__DOT__decimate_counter = 0U;
                vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable = 1U;
            } else {
                vlSelfRef.__Vdly__cic_decimator_tb__DOT__dut__DOT__decimate_counter 
                    = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_counter)));
                vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable = 0U;
            }
        } else {
            vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable = 0U;
        }
    } else {
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__output_register = 0U;
        vlSelfRef.__Vdly__cic_decimator_tb__DOT__dut__DOT__decimate_counter = 0U;
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable = 0U;
    }
    if (vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v0) {
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb[0U] 
            = vlSelfRef.__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v0;
    }
    if (vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v1) {
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb[0U] = 0ULL;
    }
    if (vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v2) {
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb[1U] 
            = vlSelfRef.__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v2;
    }
    if (vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v3) {
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb[1U] = 0ULL;
    }
    if (vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v4) {
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb[2U] 
            = vlSelfRef.__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v4;
    }
    if (vlSelfRef.__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v5) {
        vlSelfRef.cic_decimator_tb__DOT__dut__DOT__comb[2U] = 0ULL;
    }
    vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_counter 
        = vlSelfRef.__Vdly__cic_decimator_tb__DOT__dut__DOT__decimate_counter;
}

void Vcic_decimator_tb___024root___eval_nba(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_nba\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vcic_decimator_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vcic_decimator_tb___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vcic_decimator_tb___024root___nba_sequent__TOP__2(vlSelf);
    }
}

void Vcic_decimator_tb___024root___timing_commit(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___timing_commit\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((! (8ULL & vlSelfRef.__VactTriggered[0U]))) {
        vlSelfRef.__VtrigSched_h4b095651__0.commit(
                                                   "@( cic_decimator_tb.dut.decimate_enable)");
    }
}

void Vcic_decimator_tb___024root___timing_resume(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___timing_resume\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((8ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h4b095651__0.resume(
                                                   "@( cic_decimator_tb.dut.decimate_enable)");
    }
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vcic_decimator_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vcic_decimator_tb___024root___eval_phase__act(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_phase__act\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vcic_decimator_tb___024root___eval_triggers__act(vlSelf);
    Vcic_decimator_tb___024root___timing_commit(vlSelf);
    Vcic_decimator_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vcic_decimator_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vcic_decimator_tb___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

void Vcic_decimator_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vcic_decimator_tb___024root___eval_phase__nba(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_phase__nba\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vcic_decimator_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vcic_decimator_tb___024root___eval_nba(vlSelf);
        Vcic_decimator_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vcic_decimator_tb___024root___eval(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vcic_decimator_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/cic_decimator_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vcic_decimator_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/cic_decimator_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vcic_decimator_tb___024root___eval_phase__act(vlSelf));
    } while (Vcic_decimator_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vcic_decimator_tb___024root___eval_debug_assertions(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_debug_assertions\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
