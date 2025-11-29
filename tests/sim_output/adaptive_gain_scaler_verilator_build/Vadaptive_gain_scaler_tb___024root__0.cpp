// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vadaptive_gain_scaler_tb.h for the primary calling header

#include "Vadaptive_gain_scaler_tb__pch.h"

VlCoroutine Vadaptive_gain_scaler_tb___024root___eval_initial__TOP__Vtiming__0(Vadaptive_gain_scaler_tb___024root* vlSelf);
VlCoroutine Vadaptive_gain_scaler_tb___024root___eval_initial__TOP__Vtiming__1(Vadaptive_gain_scaler_tb___024root* vlSelf);

void Vadaptive_gain_scaler_tb___024root___eval_initial(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_initial\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vadaptive_gain_scaler_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vadaptive_gain_scaler_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vadaptive_gain_scaler_tb___024root___eval_initial__TOP__Vtiming__0(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.adaptive_gain_scaler_tb__DOT__clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "verilog/adaptive_gain_scaler_tb.v", 
                                             32);
        vlSelfRef.adaptive_gain_scaler_tb__DOT__clk 
            = (1U & (~ (IData)(vlSelfRef.adaptive_gain_scaler_tb__DOT__clk)));
    }
}

VlCoroutine Vadaptive_gain_scaler_tb___024root___eval_initial__TOP__Vtiming__1(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.adaptive_gain_scaler_tb__DOT__rst_n = 0U;
    vlSelfRef.adaptive_gain_scaler_tb__DOT__sample_in = 0U;
    vlSelfRef.adaptive_gain_scaler_tb__DOT__sample_valid_in = 0U;
    vlSelfRef.adaptive_gain_scaler_tb__DOT__gain_control = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/adaptive_gain_scaler_tb.v", 
                                         44);
    vlSelfRef.adaptive_gain_scaler_tb__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/adaptive_gain_scaler_tb.v", 
                                         48);
    VL_WRITEF_NX("Testing Adaptive Gain Scaler...\n",0);
    vlSelfRef.adaptive_gain_scaler_tb__DOT__gain_control = 0x81U;
    vlSelfRef.adaptive_gain_scaler_tb__DOT__sample_in = 0x00001000U;
    vlSelfRef.adaptive_gain_scaler_tb__DOT__sample_valid_in = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "verilog/adaptive_gain_scaler_tb.v", 
                                         56);
    vlSelfRef.adaptive_gain_scaler_tb__DOT__sample_valid_in = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/adaptive_gain_scaler_tb.v", 
                                         59);
    VL_WRITEF_NX("Gain Scalar test: in=0x00001000, out=0x%x\nAdaptive Gain Scaler test completed\n",0,
                 32,vlSelfRef.adaptive_gain_scaler_tb__DOT__sample_out);
    if (vlSelfRef.adaptive_gain_scaler_tb__DOT__dut__DOT__sample_valid_out_reg) {
        VL_WRITEF_NX("\342\234\223 Scaling and gain functional\nTEST PASSED\n",0);
    } else {
        VL_WRITEF_NX("\342\232\240 Output not valid\nTEST FAILED\n",0);
    }
    VL_FINISH_MT("verilog/adaptive_gain_scaler_tb.v", 72, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vadaptive_gain_scaler_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vadaptive_gain_scaler_tb___024root___eval_triggers__act(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_triggers__act\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 2U) 
                                                     | ((((~ (IData)(vlSelfRef.adaptive_gain_scaler_tb__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__adaptive_gain_scaler_tb__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.adaptive_gain_scaler_tb__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__adaptive_gain_scaler_tb__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__adaptive_gain_scaler_tb__DOT__clk__0 
        = vlSelfRef.adaptive_gain_scaler_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__adaptive_gain_scaler_tb__DOT__rst_n__0 
        = vlSelfRef.adaptive_gain_scaler_tb__DOT__rst_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vadaptive_gain_scaler_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vadaptive_gain_scaler_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___trigger_anySet__act\n"); );
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

void Vadaptive_gain_scaler_tb___024root___act_sequent__TOP__0(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___act_sequent__TOP__0\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    QData/*63:0*/ adaptive_gain_scaler_tb__DOT__dut__DOT__gained_sample;
    adaptive_gain_scaler_tb__DOT__dut__DOT__gained_sample = 0;
    // Body
    adaptive_gain_scaler_tb__DOT__dut__DOT__gained_sample 
        = VL_MULS_QQQ(64, VL_EXTENDS_QI(64,32, vlSelfRef.adaptive_gain_scaler_tb__DOT__sample_in), 
                      VL_EXTENDS_QI(64,32, ((IData)(1U) 
                                            + (0x0000000fU 
                                               & ((IData)(vlSelfRef.adaptive_gain_scaler_tb__DOT__gain_control) 
                                                  >> 4U)))));
    vlSelfRef.adaptive_gain_scaler_tb__DOT__sample_out 
        = (IData)(((8U == (0x0000000fU & (IData)(vlSelfRef.adaptive_gain_scaler_tb__DOT__gain_control)))
                    ? (QData)((IData)(adaptive_gain_scaler_tb__DOT__dut__DOT__gained_sample))
                    : ((8U > (0x0000000fU & (IData)(vlSelfRef.adaptive_gain_scaler_tb__DOT__gain_control)))
                        ? (adaptive_gain_scaler_tb__DOT__dut__DOT__gained_sample 
                           >> (0x0000000fU & (IData)(vlSelfRef.adaptive_gain_scaler_tb__DOT__gain_control)))
                        : ((8U < (0x0000000fU & (IData)(vlSelfRef.adaptive_gain_scaler_tb__DOT__gain_control)))
                            ? (adaptive_gain_scaler_tb__DOT__dut__DOT__gained_sample 
                               << (0x0000000fU & ((IData)(vlSelfRef.adaptive_gain_scaler_tb__DOT__gain_control) 
                                                  - (IData)(8U))))
                            : (QData)((IData)(adaptive_gain_scaler_tb__DOT__dut__DOT__gained_sample))))));
}

void Vadaptive_gain_scaler_tb___024root___eval_act(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_act\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        Vadaptive_gain_scaler_tb___024root___act_sequent__TOP__0(vlSelf);
    }
}

void Vadaptive_gain_scaler_tb___024root___nba_sequent__TOP__1(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___nba_sequent__TOP__1\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.adaptive_gain_scaler_tb__DOT__rst_n) {
        if (vlSelfRef.adaptive_gain_scaler_tb__DOT__sample_valid_in) {
            vlSelfRef.adaptive_gain_scaler_tb__DOT__dut__DOT__sample_valid_out_reg = 1U;
        }
    } else {
        vlSelfRef.adaptive_gain_scaler_tb__DOT__dut__DOT__sample_valid_out_reg = 0U;
    }
}

void Vadaptive_gain_scaler_tb___024root___eval_nba(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_nba\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vadaptive_gain_scaler_tb___024root___act_sequent__TOP__0(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vadaptive_gain_scaler_tb___024root___nba_sequent__TOP__1(vlSelf);
    }
}

void Vadaptive_gain_scaler_tb___024root___timing_resume(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___timing_resume\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vadaptive_gain_scaler_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vadaptive_gain_scaler_tb___024root___eval_phase__act(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_phase__act\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vadaptive_gain_scaler_tb___024root___eval_triggers__act(vlSelf);
    Vadaptive_gain_scaler_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vadaptive_gain_scaler_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vadaptive_gain_scaler_tb___024root___timing_resume(vlSelf);
        Vadaptive_gain_scaler_tb___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

void Vadaptive_gain_scaler_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vadaptive_gain_scaler_tb___024root___eval_phase__nba(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_phase__nba\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vadaptive_gain_scaler_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vadaptive_gain_scaler_tb___024root___eval_nba(vlSelf);
        Vadaptive_gain_scaler_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vadaptive_gain_scaler_tb___024root___eval(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vadaptive_gain_scaler_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/adaptive_gain_scaler_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vadaptive_gain_scaler_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/adaptive_gain_scaler_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vadaptive_gain_scaler_tb___024root___eval_phase__act(vlSelf));
    } while (Vadaptive_gain_scaler_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vadaptive_gain_scaler_tb___024root___eval_debug_assertions(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_debug_assertions\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
