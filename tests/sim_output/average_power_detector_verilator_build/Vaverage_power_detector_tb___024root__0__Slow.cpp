// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vaverage_power_detector_tb.h for the primary calling header

#include "Vaverage_power_detector_tb__pch.h"

VL_ATTR_COLD void Vaverage_power_detector_tb___024root___eval_static(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_static\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__average_power_detector_tb__DOT__clk__0 
        = vlSelfRef.average_power_detector_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__average_power_detector_tb__DOT__rst_n__0 
        = vlSelfRef.average_power_detector_tb__DOT__rst_n;
}

VL_ATTR_COLD void Vaverage_power_detector_tb___024root___eval_final(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_final\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vaverage_power_detector_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vaverage_power_detector_tb___024root___eval_phase__stl(Vaverage_power_detector_tb___024root* vlSelf);

VL_ATTR_COLD void Vaverage_power_detector_tb___024root___eval_settle(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_settle\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vaverage_power_detector_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("verilog/average_power_detector_tb.v", 3, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vaverage_power_detector_tb___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vaverage_power_detector_tb___024root___eval_triggers__stl(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_triggers__stl\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vaverage_power_detector_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vaverage_power_detector_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vaverage_power_detector_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vaverage_power_detector_tb___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vaverage_power_detector_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___trigger_anySet__stl\n"); );
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

void Vaverage_power_detector_tb___024root___act_sequent__TOP__0(Vaverage_power_detector_tb___024root* vlSelf);

VL_ATTR_COLD void Vaverage_power_detector_tb___024root___eval_stl(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_stl\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vaverage_power_detector_tb___024root___act_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vaverage_power_detector_tb___024root___eval_phase__stl(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___eval_phase__stl\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vaverage_power_detector_tb___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vaverage_power_detector_tb___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vaverage_power_detector_tb___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vaverage_power_detector_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vaverage_power_detector_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vaverage_power_detector_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge average_power_detector_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge average_power_detector_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vaverage_power_detector_tb___024root___ctor_var_reset(Vaverage_power_detector_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaverage_power_detector_tb___024root___ctor_var_reset\n"); );
    Vaverage_power_detector_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->average_power_detector_tb__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5133308954888242701ull);
    vlSelf->average_power_detector_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14760896373715168956ull);
    vlSelf->average_power_detector_tb__DOT__sample_in = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14274211755720403664ull);
    vlSelf->average_power_detector_tb__DOT__sample_valid_in = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15549371581012475063ull);
    vlSelf->average_power_detector_tb__DOT__dut__DOT__sample_squared = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 1216909466232814600ull);
    VL_SCOPED_RAND_RESET_W(74, vlSelf->average_power_detector_tb__DOT__dut__DOT__sum_of_squares, __VscopeHash, 18181281855107383035ull);
    vlSelf->average_power_detector_tb__DOT__dut__DOT__sample_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 5184863464528939719ull);
    vlSelf->average_power_detector_tb__DOT__dut__DOT__output_valid_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14580922390478261154ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__average_power_detector_tb__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16111180606195785161ull);
    vlSelf->__Vtrigprevexpr___TOP__average_power_detector_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14870572790881120426ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
