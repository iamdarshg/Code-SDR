// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vaverage_power_detector_tb.h for the primary calling header

#include "Vaverage_power_detector_tb__pch.h"

VlCoroutine Vaverage_power_detector_tb___024root___eval_initial__TOP__Vtiming__0(Vaverage_power_detector_tb___024root* vlSelf);
VlCoroutine Vaverage_power_detector_tb___024root___eval_initial__TOP__Vtiming__1(Vaverage_power_detector_tb___024root* vlSelf);

void Vaverage_power_detector_tb___024root___eval_initial(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_initial\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vaverage_power_detector_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vaverage_power_detector_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vaverage_power_detector_tb___024root___eval_initial__TOP__Vtiming__0(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.average_power_detector_tb__DOT__clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "verilog/average_power_detector_tb.v", 
                                             32);
        vlSelfRef.average_power_detector_tb__DOT__clk 
            = (1U & (~ (IData)(vlSelfRef.average_power_detector_tb__DOT__clk)));
    }
}

VlCoroutine Vaverage_power_detector_tb___024root___eval_initial__TOP__Vtiming__1(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ average_power_detector_tb__DOT__i;
    average_power_detector_tb__DOT__i = 0;
    // Body
    vlSelfRef.average_power_detector_tb__DOT__rst_n = 0U;
    vlSelfRef.average_power_detector_tb__DOT__sample_in = 0U;
    vlSelfRef.average_power_detector_tb__DOT__sample_valid_in = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/average_power_detector_tb.v", 
                                         43);
    vlSelfRef.average_power_detector_tb__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/average_power_detector_tb.v", 
                                         47);
    VL_WRITEF_NX("Testing Average Power Detector...\n",0);
    average_power_detector_tb__DOT__i = 0U;
    while ((0x00000800U > average_power_detector_tb__DOT__i)) {
        vlSelfRef.average_power_detector_tb__DOT__sample_in 
            = (0x000000ffU & average_power_detector_tb__DOT__i);
        vlSelfRef.average_power_detector_tb__DOT__sample_valid_in = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                             nullptr, 
                                             "verilog/average_power_detector_tb.v", 
                                             55);
        average_power_detector_tb__DOT__i = ((IData)(1U) 
                                             + average_power_detector_tb__DOT__i);
    }
    vlSelfRef.average_power_detector_tb__DOT__sample_valid_in = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000f4240ULL, 
                                         nullptr, "verilog/average_power_detector_tb.v", 
                                         60);
    VL_WRITEF_NX("Average Power Detector test completed\n",0);
    if (vlSelfRef.average_power_detector_tb__DOT__dut__DOT__output_valid_reg) {
        VL_WRITEF_NX("(OK) Average power calculation functional: 0x%x\nTEST PASSED\n",0,
                     32,((vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sum_of_squares[1U] 
                          << 0x00000016U) | (vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sum_of_squares[0U] 
                                             >> 0x0000000aU)));
    } else {
        VL_WRITEF_NX("\342\232\240 No valid average power output\nTEST FAILED\n",0);
    }
    VL_FINISH_MT("verilog/average_power_detector_tb.v", 73, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vaverage_power_detector_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vaverage_power_detector_tb___024root___eval_triggers__act(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_triggers__act\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 2U) 
                                                     | ((((~ (IData)(vlSelfRef.average_power_detector_tb__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__average_power_detector_tb__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.average_power_detector_tb__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__average_power_detector_tb__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__average_power_detector_tb__DOT__clk__0 
        = vlSelfRef.average_power_detector_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__average_power_detector_tb__DOT__rst_n__0 
        = vlSelfRef.average_power_detector_tb__DOT__rst_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vaverage_power_detector_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vaverage_power_detector_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___trigger_anySet__act\n"); );
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

void Vaverage_power_detector_tb___024root___act_sequent__TOP__0(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___act_sequent__TOP__0\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sample_squared 
        = VL_MULS_QQQ(64, VL_EXTENDS_QI(64,32, vlSelfRef.average_power_detector_tb__DOT__sample_in), 
                      VL_EXTENDS_QI(64,32, vlSelfRef.average_power_detector_tb__DOT__sample_in));
}

void Vaverage_power_detector_tb___024root___eval_act(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_act\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        Vaverage_power_detector_tb___024root___act_sequent__TOP__0(vlSelf);
    }
}

void Vaverage_power_detector_tb___024root___nba_sequent__TOP__0(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___nba_sequent__TOP__0\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlWide<3>/*73:0*/ __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares;
    VL_ZERO_W(74, __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares);
    SData/*9:0*/ __Vdly__average_power_detector_tb__DOT__dut__DOT__sample_count;
    __Vdly__average_power_detector_tb__DOT__dut__DOT__sample_count = 0;
    VlWide<3>/*95:0*/ __Vtemp_2;
    VlWide<3>/*95:0*/ __Vtemp_3;
    // Body
    __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[0U] 
        = vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sum_of_squares[0U];
    __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[1U] 
        = vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sum_of_squares[1U];
    __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[2U] 
        = vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sum_of_squares[2U];
    __Vdly__average_power_detector_tb__DOT__dut__DOT__sample_count 
        = vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sample_count;
    if (vlSelfRef.average_power_detector_tb__DOT__rst_n) {
        vlSelfRef.average_power_detector_tb__DOT__dut__DOT__output_valid_reg = 0U;
        if (vlSelfRef.average_power_detector_tb__DOT__sample_valid_in) {
            if ((0x03ffU == (IData)(vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sample_count))) {
                __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[0U] 
                    = (IData)(vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sample_squared);
                __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[1U] 
                    = (IData)((vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sample_squared 
                               >> 0x00000020U));
                __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[2U] = 0U;
                __Vdly__average_power_detector_tb__DOT__dut__DOT__sample_count = 0U;
                vlSelfRef.average_power_detector_tb__DOT__dut__DOT__output_valid_reg = 1U;
            } else {
                __Vtemp_2[0U] = (IData)(vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sample_squared);
                __Vtemp_2[1U] = (IData)((vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sample_squared 
                                         >> 0x00000020U));
                __Vtemp_2[2U] = 0U;
                VL_ADD_W(3, __Vtemp_3, vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sum_of_squares, __Vtemp_2);
                __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[0U] 
                    = __Vtemp_3[0U];
                __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[1U] 
                    = __Vtemp_3[1U];
                __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[2U] 
                    = (0x000003ffU & __Vtemp_3[2U]);
                __Vdly__average_power_detector_tb__DOT__dut__DOT__sample_count 
                    = (0x000003ffU & ((IData)(1U) + (IData)(vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sample_count)));
            }
        }
    } else {
        __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[0U] = 0U;
        __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[1U] = 0U;
        __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[2U] = 0U;
        __Vdly__average_power_detector_tb__DOT__dut__DOT__sample_count = 0U;
        vlSelfRef.average_power_detector_tb__DOT__dut__DOT__output_valid_reg = 0U;
    }
    vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sum_of_squares[0U] 
        = __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[0U];
    vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sum_of_squares[1U] 
        = __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[1U];
    vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sum_of_squares[2U] 
        = __Vdly__average_power_detector_tb__DOT__dut__DOT__sum_of_squares[2U];
    vlSelfRef.average_power_detector_tb__DOT__dut__DOT__sample_count 
        = __Vdly__average_power_detector_tb__DOT__dut__DOT__sample_count;
}

void Vaverage_power_detector_tb___024root___eval_nba(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_nba\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vaverage_power_detector_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((4ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vaverage_power_detector_tb___024root___act_sequent__TOP__0(vlSelf);
    }
}

void Vaverage_power_detector_tb___024root___timing_resume(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___timing_resume\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vaverage_power_detector_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vaverage_power_detector_tb___024root___eval_phase__act(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_phase__act\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vaverage_power_detector_tb___024root___eval_triggers__act(vlSelf);
    Vaverage_power_detector_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vaverage_power_detector_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vaverage_power_detector_tb___024root___timing_resume(vlSelf);
        Vaverage_power_detector_tb___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

void Vaverage_power_detector_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vaverage_power_detector_tb___024root___eval_phase__nba(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_phase__nba\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vaverage_power_detector_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vaverage_power_detector_tb___024root___eval_nba(vlSelf);
        Vaverage_power_detector_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vaverage_power_detector_tb___024root___eval(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vaverage_power_detector_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/average_power_detector_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vaverage_power_detector_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/average_power_detector_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vaverage_power_detector_tb___024root___eval_phase__act(vlSelf));
    } while (Vaverage_power_detector_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vaverage_power_detector_tb___024root___eval_debug_assertions(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_debug_assertions\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
