// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vnco_generator_tb.h for the primary calling header

#include "Vnco_generator_tb__pch.h"

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_initial__TOP(Vnco_generator_tb___024root* vlSelf);
VlCoroutine Vnco_generator_tb___024root___eval_initial__TOP__Vtiming__0(Vnco_generator_tb___024root* vlSelf);
VlCoroutine Vnco_generator_tb___024root___eval_initial__TOP__Vtiming__1(Vnco_generator_tb___024root* vlSelf);

void Vnco_generator_tb___024root___eval_initial(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_initial\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vnco_generator_tb___024root___eval_initial__TOP(vlSelf);
    Vnco_generator_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vnco_generator_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vnco_generator_tb___024root___eval_initial__TOP__Vtiming__0(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.nco_generator_tb__DOT__clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "verilog/nco_generator_tb.v", 
                                             34);
        vlSelfRef.nco_generator_tb__DOT__clk = (1U 
                                                & (~ (IData)(vlSelfRef.nco_generator_tb__DOT__clk)));
    }
}

VlCoroutine Vnco_generator_tb___024root___eval_initial__TOP__Vtiming__1(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.nco_generator_tb__DOT__rst_n = 0U;
    vlSelfRef.nco_generator_tb__DOT__frequency_word = 0U;
    vlSelfRef.nco_generator_tb__DOT__enable = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/nco_generator_tb.v", 
                                         45);
    vlSelfRef.nco_generator_tb__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/nco_generator_tb.v", 
                                         49);
    VL_WRITEF_NX("Testing NCO generator...\n",0);
    vlSelfRef.nco_generator_tb__DOT__frequency_word = 0x00100000U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000f4240ULL, 
                                         nullptr, "verilog/nco_generator_tb.v", 
                                         58);
    vlSelfRef.nco_generator_tb__DOT__frequency_word = 0x00200000U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000f4240ULL, 
                                         nullptr, "verilog/nco_generator_tb.v", 
                                         64);
    vlSelfRef.nco_generator_tb__DOT__frequency_word = 0x10000000U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000f4240ULL, 
                                         nullptr, "verilog/nco_generator_tb.v", 
                                         69);
    VL_WRITEF_NX("NCO generator test completed\n\342\234\223 Frequency synthesis working\nTEST PASSED\n",0);
    VL_FINISH_MT("verilog/nco_generator_tb.v", 76, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vnco_generator_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vnco_generator_tb___024root___eval_triggers__act(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_triggers__act\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 2U) 
                                                     | ((((~ (IData)(vlSelfRef.nco_generator_tb__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__nco_generator_tb__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.nco_generator_tb__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__nco_generator_tb__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__nco_generator_tb__DOT__clk__0 
        = vlSelfRef.nco_generator_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__nco_generator_tb__DOT__rst_n__0 
        = vlSelfRef.nco_generator_tb__DOT__rst_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vnco_generator_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vnco_generator_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___trigger_anySet__act\n"); );
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

void Vnco_generator_tb___024root___act_sequent__TOP__0(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___act_sequent__TOP__0\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator_next 
        = (vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
           + ((IData)(vlSelfRef.nco_generator_tb__DOT__enable)
               ? vlSelfRef.nco_generator_tb__DOT__frequency_word
               : 0U));
}

void Vnco_generator_tb___024root___eval_act(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_act\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        Vnco_generator_tb___024root___act_sequent__TOP__0(vlSelf);
    }
}

void Vnco_generator_tb___024root___nba_sequent__TOP__0(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___nba_sequent__TOP__0\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((0x00000000000000c8ULL < VL_TIME_UNITED_Q(1000))))) {
        VL_WRITEF_NX("Time=%0t: frequency_word=%x, sine=%x, cosine=%x, valid=%b\n",0,
                     64,VL_TIME_UNITED_Q(1000),-9,32,
                     vlSelfRef.nco_generator_tb__DOT__frequency_word,
                     16,(IData)(vlSelfRef.nco_generator_tb__DOT__dut__DOT__sine_reg),
                     16,vlSelfRef.nco_generator_tb__DOT__dut__DOT__cosine_reg,
                     1,(IData)(vlSelfRef.nco_generator_tb__DOT__dut__DOT__valid_reg));
    }
}

void Vnco_generator_tb___024root___nba_sequent__TOP__1(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___nba_sequent__TOP__1\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__valid_reg 
        = ((IData)(vlSelfRef.nco_generator_tb__DOT__rst_n) 
           && (IData)(vlSelfRef.nco_generator_tb__DOT__enable));
    if (vlSelfRef.nco_generator_tb__DOT__rst_n) {
        if (vlSelfRef.nco_generator_tb__DOT__enable) {
            if ((vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
                 >> 0x0000001fU)) {
                if ((0x40000000U & vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator)) {
                    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sine_reg 
                        = (0x0000ffffU & (- vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom
                                          [(0x000000ffU 
                                            & ((IData)(0xffU) 
                                               - (vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
                                                  >> 0x00000010U)))]));
                    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cosine_reg 
                        = (0x0000ffffU & vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom
                           [(0x000000ffU & ((IData)(0xffU) 
                                            - (vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
                                               >> 0x00000010U)))]);
                } else {
                    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sine_reg 
                        = (0x0000ffffU & (- vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom
                                          [(0x000000ffU 
                                            & (vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
                                               >> 0x00000010U))]));
                    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cosine_reg 
                        = (0x0000ffffU & (- vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom
                                          [(0x000000ffU 
                                            & (vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
                                               >> 0x00000010U))]));
                }
            } else if ((0x40000000U & vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator)) {
                vlSelfRef.nco_generator_tb__DOT__dut__DOT__sine_reg 
                    = (0x0000ffffU & vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom
                       [(0x000000ffU & ((IData)(0xffU) 
                                        - (vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
                                           >> 0x00000010U)))]);
                vlSelfRef.nco_generator_tb__DOT__dut__DOT__cosine_reg 
                    = (0x0000ffffU & (- vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom
                                      [(0x000000ffU 
                                        & ((IData)(0xffU) 
                                           - (vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
                                              >> 0x00000010U)))]));
            } else {
                vlSelfRef.nco_generator_tb__DOT__dut__DOT__sine_reg 
                    = (0x0000ffffU & vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom
                       [(0x000000ffU & (vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
                                        >> 0x00000010U))]);
                vlSelfRef.nco_generator_tb__DOT__dut__DOT__cosine_reg 
                    = (0x0000ffffU & vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom
                       [(0x000000ffU & (vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
                                        >> 0x00000010U))]);
            }
        }
        vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator 
            = vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator_next;
    } else {
        vlSelfRef.nco_generator_tb__DOT__dut__DOT__sine_reg = 0U;
        vlSelfRef.nco_generator_tb__DOT__dut__DOT__cosine_reg = 0U;
        vlSelfRef.nco_generator_tb__DOT__dut__DOT__phase_accumulator = 0U;
    }
}

void Vnco_generator_tb___024root___eval_nba(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_nba\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vnco_generator_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vnco_generator_tb___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((7ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vnco_generator_tb___024root___act_sequent__TOP__0(vlSelf);
    }
}

void Vnco_generator_tb___024root___timing_resume(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___timing_resume\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vnco_generator_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vnco_generator_tb___024root___eval_phase__act(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_phase__act\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vnco_generator_tb___024root___eval_triggers__act(vlSelf);
    Vnco_generator_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vnco_generator_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vnco_generator_tb___024root___timing_resume(vlSelf);
        Vnco_generator_tb___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

void Vnco_generator_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vnco_generator_tb___024root___eval_phase__nba(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_phase__nba\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vnco_generator_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vnco_generator_tb___024root___eval_nba(vlSelf);
        Vnco_generator_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vnco_generator_tb___024root___eval(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vnco_generator_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/nco_generator_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vnco_generator_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/nco_generator_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vnco_generator_tb___024root___eval_phase__act(vlSelf));
    } while (Vnco_generator_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vnco_generator_tb___024root___eval_debug_assertions(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_debug_assertions\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
