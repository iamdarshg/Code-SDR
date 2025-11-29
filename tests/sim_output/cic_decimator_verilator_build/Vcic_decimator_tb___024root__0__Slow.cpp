// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vcic_decimator_tb.h for the primary calling header

#include "Vcic_decimator_tb__pch.h"

VL_ATTR_COLD void Vcic_decimator_tb___024root___eval_static__TOP(Vcic_decimator_tb___024root* vlSelf);

VL_ATTR_COLD void Vcic_decimator_tb___024root___eval_static(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_static\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vcic_decimator_tb___024root___eval_static__TOP(vlSelf);
    vlSelfRef.__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__clk__0 
        = vlSelfRef.cic_decimator_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__rst_n__0 
        = vlSelfRef.cic_decimator_tb__DOT__rst_n;
    vlSelfRef.__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__dut__DOT__decimate_enable__0 
        = vlSelfRef.cic_decimator_tb__DOT__dut__DOT__decimate_enable;
}

VL_ATTR_COLD void Vcic_decimator_tb___024root___eval_static__TOP(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_static__TOP\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.cic_decimator_tb__DOT__test_completed = 0U;
}

VL_ATTR_COLD void Vcic_decimator_tb___024root___eval_final(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_final\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vcic_decimator_tb___024root___eval_settle(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___eval_settle\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

bool Vcic_decimator_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vcic_decimator_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vcic_decimator_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge cic_decimator_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge cic_decimator_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @( cic_decimator_tb.dut.decimate_enable)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vcic_decimator_tb___024root___ctor_var_reset(Vcic_decimator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vcic_decimator_tb___024root___ctor_var_reset\n"); );
    Vcic_decimator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->cic_decimator_tb__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17188870667260932094ull);
    vlSelf->cic_decimator_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13725508015658741926ull);
    vlSelf->cic_decimator_tb__DOT__data_in = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 17390077516233304559ull);
    vlSelf->cic_decimator_tb__DOT__data_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 602427049655000090ull);
    vlSelf->cic_decimator_tb__DOT__i = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 9785134742113226217ull);
    vlSelf->cic_decimator_tb__DOT__test_passed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12562794135668452056ull);
    vlSelf->cic_decimator_tb__DOT__test_completed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13201894304466886685ull);
    vlSelf->cic_decimator_tb__DOT__dut__DOT__comb_loop_j = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 2452899487483133840ull);
    vlSelf->cic_decimator_tb__DOT__dut__DOT__decimate_enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2208190339473495085ull);
    vlSelf->cic_decimator_tb__DOT__dut__DOT__decimate_counter = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16294716851427833837ull);
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->cic_decimator_tb__DOT__dut__DOT__comb[__Vi0] = VL_SCOPED_RAND_RESET_Q(41, __VscopeHash, 15548599313261707180ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        for (int __Vi1 = 0; __Vi1 < 2; ++__Vi1) {
            vlSelf->cic_decimator_tb__DOT__dut__DOT__comb_delay[__Vi0][__Vi1] = VL_SCOPED_RAND_RESET_Q(41, __VscopeHash, 5444895543403159201ull);
        }
    }
    vlSelf->cic_decimator_tb__DOT__dut__DOT__output_register = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 1546886421095647318ull);
    vlSelf->__Vdly__cic_decimator_tb__DOT__dut__DOT__decimate_counter = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9842274955551419449ull);
    vlSelf->__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v0 = VL_SCOPED_RAND_RESET_Q(41, __VscopeHash, 13086700221512319360ull);
    vlSelf->__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v0 = 0;
    vlSelf->__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v1 = 0;
    vlSelf->__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v2 = VL_SCOPED_RAND_RESET_Q(41, __VscopeHash, 7907788189114283571ull);
    vlSelf->__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v2 = 0;
    vlSelf->__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v3 = 0;
    vlSelf->__VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v4 = VL_SCOPED_RAND_RESET_Q(41, __VscopeHash, 1902804726423281886ull);
    vlSelf->__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v4 = 0;
    vlSelf->__VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v5 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17279965321339588894ull);
    vlSelf->__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3639053541592341803ull);
    vlSelf->__Vtrigprevexpr___TOP__cic_decimator_tb__DOT__dut__DOT__decimate_enable__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17432159663240003056ull);
    vlSelf->__VactDidInit = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
