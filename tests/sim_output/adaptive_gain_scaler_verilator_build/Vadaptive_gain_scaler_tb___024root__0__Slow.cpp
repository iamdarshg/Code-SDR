// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vadaptive_gain_scaler_tb.h for the primary calling header

#include "Vadaptive_gain_scaler_tb__pch.h"

VL_ATTR_COLD void Vadaptive_gain_scaler_tb___024root___eval_static(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_static\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__adaptive_gain_scaler_tb__DOT__clk__0 
        = vlSelfRef.adaptive_gain_scaler_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__adaptive_gain_scaler_tb__DOT__rst_n__0 
        = vlSelfRef.adaptive_gain_scaler_tb__DOT__rst_n;
}

VL_ATTR_COLD void Vadaptive_gain_scaler_tb___024root___eval_final(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_final\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vadaptive_gain_scaler_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vadaptive_gain_scaler_tb___024root___eval_phase__stl(Vadaptive_gain_scaler_tb___024root* vlSelf);

VL_ATTR_COLD void Vadaptive_gain_scaler_tb___024root___eval_settle(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_settle\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vadaptive_gain_scaler_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("verilog/adaptive_gain_scaler_tb.v", 3, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vadaptive_gain_scaler_tb___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vadaptive_gain_scaler_tb___024root___eval_triggers__stl(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_triggers__stl\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vadaptive_gain_scaler_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vadaptive_gain_scaler_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vadaptive_gain_scaler_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vadaptive_gain_scaler_tb___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vadaptive_gain_scaler_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___trigger_anySet__stl\n"); );
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

void Vadaptive_gain_scaler_tb___024root___act_sequent__TOP__0(Vadaptive_gain_scaler_tb___024root* vlSelf);

VL_ATTR_COLD void Vadaptive_gain_scaler_tb___024root___eval_stl(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_stl\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vadaptive_gain_scaler_tb___024root___act_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vadaptive_gain_scaler_tb___024root___eval_phase__stl(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___eval_phase__stl\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vadaptive_gain_scaler_tb___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vadaptive_gain_scaler_tb___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vadaptive_gain_scaler_tb___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vadaptive_gain_scaler_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vadaptive_gain_scaler_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vadaptive_gain_scaler_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge adaptive_gain_scaler_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge adaptive_gain_scaler_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vadaptive_gain_scaler_tb___024root___ctor_var_reset(Vadaptive_gain_scaler_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadaptive_gain_scaler_tb___024root___ctor_var_reset\n"); );
    Vadaptive_gain_scaler_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->adaptive_gain_scaler_tb__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1108565919240416183ull);
    vlSelf->adaptive_gain_scaler_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15344004839336390088ull);
    vlSelf->adaptive_gain_scaler_tb__DOT__sample_in = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12160750731129377828ull);
    vlSelf->adaptive_gain_scaler_tb__DOT__sample_valid_in = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3973248409589987283ull);
    vlSelf->adaptive_gain_scaler_tb__DOT__gain_control = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12801788667612128142ull);
    vlSelf->adaptive_gain_scaler_tb__DOT__sample_out = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 16869050894437580691ull);
    vlSelf->adaptive_gain_scaler_tb__DOT__dut__DOT__sample_valid_out_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5246450994111481331ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__adaptive_gain_scaler_tb__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17515918888237246161ull);
    vlSelf->__Vtrigprevexpr___TOP__adaptive_gain_scaler_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2705664372419085538ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
