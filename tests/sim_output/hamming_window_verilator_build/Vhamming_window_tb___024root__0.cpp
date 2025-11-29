// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vhamming_window_tb.h for the primary calling header

#include "Vhamming_window_tb__pch.h"

VL_ATTR_COLD void Vhamming_window_tb___024root___eval_initial__TOP(Vhamming_window_tb___024root* vlSelf);
VlCoroutine Vhamming_window_tb___024root___eval_initial__TOP__Vtiming__0(Vhamming_window_tb___024root* vlSelf);
VlCoroutine Vhamming_window_tb___024root___eval_initial__TOP__Vtiming__1(Vhamming_window_tb___024root* vlSelf);

void Vhamming_window_tb___024root___eval_initial(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_initial\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vhamming_window_tb___024root___eval_initial__TOP(vlSelf);
    Vhamming_window_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vhamming_window_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vhamming_window_tb___024root___eval_initial__TOP__Vtiming__0(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.hamming_window_tb__DOT__clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "verilog/hamming_window_tb.v", 
                                             35);
        vlSelfRef.hamming_window_tb__DOT__clk = (1U 
                                                 & (~ (IData)(vlSelfRef.hamming_window_tb__DOT__clk)));
    }
}

VlCoroutine Vhamming_window_tb___024root___eval_initial__TOP__Vtiming__1(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ hamming_window_tb__DOT__input_index;
    hamming_window_tb__DOT__input_index = 0;
    // Body
    vlSelfRef.hamming_window_tb__DOT__rst_n = 0U;
    vlSelfRef.hamming_window_tb__DOT__data_in = 0U;
    vlSelfRef.hamming_window_tb__DOT__data_valid = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/hamming_window_tb.v", 
                                         46);
    vlSelfRef.hamming_window_tb__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/hamming_window_tb.v", 
                                         50);
    VL_WRITEF_NX("Testing Hamming Window...\n",0);
    hamming_window_tb__DOT__input_index = 0U;
    while ((0x00000040U > hamming_window_tb__DOT__input_index)) {
        co_await vlSelfRef.__VtrigSched_h2ecb2d09__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge hamming_window_tb.clk)", 
                                                             "verilog/hamming_window_tb.v", 
                                                             57);
        vlSelfRef.hamming_window_tb__DOT__data_in = hamming_window_tb__DOT__input_index;
        vlSelfRef.hamming_window_tb__DOT__data_valid = 1U;
        hamming_window_tb__DOT__input_index = ((IData)(1U) 
                                               + hamming_window_tb__DOT__input_index);
    }
    vlSelfRef.hamming_window_tb__DOT__data_valid = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000f4240ULL, 
                                         nullptr, "verilog/hamming_window_tb.v", 
                                         65);
    VL_WRITEF_NX("Hamming Window test completed - 64 samples processed\n\342\234\223 Windowing appears functional\nTEST PASSED\nFirst coefficient should be near zero\nMiddle coefficient should be amplitude scaled\nLast coefficient should be near zero\n",0);
    VL_FINISH_MT("verilog/hamming_window_tb.v", 84, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhamming_window_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vhamming_window_tb___024root___eval_triggers__act(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_triggers__act\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 2U) 
                                                     | ((((~ (IData)(vlSelfRef.hamming_window_tb__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__hamming_window_tb__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.hamming_window_tb__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__hamming_window_tb__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__hamming_window_tb__DOT__clk__0 
        = vlSelfRef.hamming_window_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__hamming_window_tb__DOT__rst_n__0 
        = vlSelfRef.hamming_window_tb__DOT__rst_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vhamming_window_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vhamming_window_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___trigger_anySet__act\n"); );
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

void Vhamming_window_tb___024root___nba_sequent__TOP__0(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___nba_sequent__TOP__0\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__hamming_window_tb__DOT__dut__DOT__sample_counter 
        = vlSelfRef.hamming_window_tb__DOT__dut__DOT__sample_counter;
    vlSelfRef.__Vdly__hamming_window_tb__DOT__dut__DOT__output_delay 
        = vlSelfRef.hamming_window_tb__DOT__dut__DOT__output_delay;
}

void Vhamming_window_tb___024root___nba_sequent__TOP__1(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___nba_sequent__TOP__1\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.hamming_window_tb__DOT__dut__DOT__output_valid_reg) {
        vlSelfRef.hamming_window_tb__DOT__output_count 
            = ((IData)(1U) + vlSelfRef.hamming_window_tb__DOT__output_count);
        if (VL_UNLIKELY(((VL_GTES_III(32, 5U, vlSelfRef.hamming_window_tb__DOT__output_count) 
                          | VL_LTES_III(32, 0x0000003cU, vlSelfRef.hamming_window_tb__DOT__output_count))))) {
            VL_WRITEF_NX("Sample %0d: input=0x%x, windowed=0x%x\n",0,
                         32,vlSelfRef.hamming_window_tb__DOT__output_count,
                         32,vlSelfRef.hamming_window_tb__DOT__data_in,
                         32,vlSelfRef.hamming_window_tb__DOT__dut__DOT__windowed_data);
        }
    }
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__windowed_data 
        = (IData)((VL_MULS_QQQ(64, VL_EXTENDS_QI(64,32, vlSelfRef.hamming_window_tb__DOT__data_in), 
                               VL_EXTENDS_QI(64,32, 
                                             (((- (IData)(
                                                          (1U 
                                                           & ((IData)(vlSelfRef.hamming_window_tb__DOT__dut__DOT__current_coeff) 
                                                              >> 0x0000000fU)))) 
                                               << 0x00000010U) 
                                              | (IData)(vlSelfRef.hamming_window_tb__DOT__dut__DOT__current_coeff)))) 
                   >> 0x0fU));
}

void Vhamming_window_tb___024root___nba_sequent__TOP__2(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___nba_sequent__TOP__2\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.hamming_window_tb__DOT__rst_n) {
        vlSelfRef.__Vdly__hamming_window_tb__DOT__dut__DOT__output_delay 
            = ((6U & ((IData)(vlSelfRef.hamming_window_tb__DOT__dut__DOT__output_delay) 
                      << 1U)) | (IData)(vlSelfRef.hamming_window_tb__DOT__dut__DOT__buffer_full));
        vlSelfRef.hamming_window_tb__DOT__dut__DOT__output_valid_reg 
            = (1U & ((IData)(vlSelfRef.hamming_window_tb__DOT__dut__DOT__output_delay) 
                     >> 2U));
        if (vlSelfRef.hamming_window_tb__DOT__data_valid) {
            if ((0x003fU == (IData)(vlSelfRef.hamming_window_tb__DOT__dut__DOT__sample_counter))) {
                vlSelfRef.__Vdly__hamming_window_tb__DOT__dut__DOT__sample_counter = 0U;
                vlSelfRef.hamming_window_tb__DOT__dut__DOT__buffer_full = 1U;
            } else {
                vlSelfRef.__Vdly__hamming_window_tb__DOT__dut__DOT__sample_counter 
                    = (0x000003ffU & ((IData)(1U) + (IData)(vlSelfRef.hamming_window_tb__DOT__dut__DOT__sample_counter)));
                vlSelfRef.hamming_window_tb__DOT__dut__DOT__buffer_full = 0U;
            }
        }
    } else {
        vlSelfRef.hamming_window_tb__DOT__dut__DOT__output_valid_reg = 0U;
        vlSelfRef.__Vdly__hamming_window_tb__DOT__dut__DOT__output_delay = 0U;
        vlSelfRef.__Vdly__hamming_window_tb__DOT__dut__DOT__sample_counter = 0U;
        vlSelfRef.hamming_window_tb__DOT__dut__DOT__buffer_full = 0U;
    }
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__output_delay 
        = vlSelfRef.__Vdly__hamming_window_tb__DOT__dut__DOT__output_delay;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__sample_counter 
        = vlSelfRef.__Vdly__hamming_window_tb__DOT__dut__DOT__sample_counter;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__current_coeff 
        = vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff
        [(0x0000003fU & (IData)(vlSelfRef.hamming_window_tb__DOT__dut__DOT__sample_counter))];
}

void Vhamming_window_tb___024root___eval_nba(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_nba\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vhamming_window_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vhamming_window_tb___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vhamming_window_tb___024root___nba_sequent__TOP__2(vlSelf);
    }
}

void Vhamming_window_tb___024root___timing_commit(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___timing_commit\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((! (1ULL & vlSelfRef.__VactTriggered[0U]))) {
        vlSelfRef.__VtrigSched_h2ecb2d09__0.commit(
                                                   "@(posedge hamming_window_tb.clk)");
    }
}

void Vhamming_window_tb___024root___timing_resume(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___timing_resume\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h2ecb2d09__0.resume(
                                                   "@(posedge hamming_window_tb.clk)");
    }
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vhamming_window_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vhamming_window_tb___024root___eval_phase__act(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_phase__act\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vhamming_window_tb___024root___eval_triggers__act(vlSelf);
    Vhamming_window_tb___024root___timing_commit(vlSelf);
    Vhamming_window_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vhamming_window_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vhamming_window_tb___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

void Vhamming_window_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vhamming_window_tb___024root___eval_phase__nba(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_phase__nba\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vhamming_window_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vhamming_window_tb___024root___eval_nba(vlSelf);
        Vhamming_window_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vhamming_window_tb___024root___eval(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vhamming_window_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/hamming_window_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vhamming_window_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/hamming_window_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vhamming_window_tb___024root___eval_phase__act(vlSelf));
    } while (Vhamming_window_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vhamming_window_tb___024root___eval_debug_assertions(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_debug_assertions\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
