// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vadc_interface_tb.h for the primary calling header

#include "Vadc_interface_tb__pch.h"

VL_ATTR_COLD void Vadc_interface_tb___024root___eval_static(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_static\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__adc_interface_tb__DOT__clk_adc__0 
        = vlSelfRef.adc_interface_tb__DOT__clk_adc;
    vlSelfRef.__Vtrigprevexpr___TOP__adc_interface_tb__DOT__rst_n__0 
        = vlSelfRef.adc_interface_tb__DOT__rst_n;
}

VL_ATTR_COLD void Vadc_interface_tb___024root___eval_final(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_final\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vadc_interface_tb___024root___eval_settle(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_settle\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

bool Vadc_interface_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vadc_interface_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vadc_interface_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge adc_interface_tb.clk_adc)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge adc_interface_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vadc_interface_tb___024root___ctor_var_reset(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___ctor_var_reset\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->adc_interface_tb__DOT__clk_adc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9438585214687122641ull);
    vlSelf->adc_interface_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5550966429233748475ull);
    vlSelf->adc_interface_tb__DOT__adc_data = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 14661574324583049512ull);
    vlSelf->adc_interface_tb__DOT__adc_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17299888638044821826ull);
    vlSelf->adc_interface_tb__DOT__adc_ovr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13753382653575613999ull);
    vlSelf->adc_interface_tb__DOT__dut__DOT__adc_data_reg = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 17723158096118499441ull);
    vlSelf->adc_interface_tb__DOT__dut__DOT__adc_valid_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2590166496224638632ull);
    vlSelf->adc_interface_tb__DOT__dut__DOT__adc_ovr_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6327396397199187828ull);
    vlSelf->adc_interface_tb__DOT__dut__DOT__overflow_count = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 7325180761837512534ull);
    vlSelf->adc_interface_tb__DOT__dut__DOT__overflow_detect_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4106507753424534283ull);
    vlSelf->adc_interface_tb__DOT__dut__DOT__dc_offset = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14308491987877641367ull);
    vlSelf->adc_interface_tb__DOT__dut__DOT__avg_count = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13049923070585104750ull);
    vlSelf->adc_interface_tb__DOT__dut__DOT__sample_valid_sync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2611843832605286429ull);
    vlSelf->__Vdly__adc_interface_tb__DOT__dut__DOT__dc_offset = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 2941591163052798130ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__adc_interface_tb__DOT__clk_adc__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18351343700800956724ull);
    vlSelf->__Vtrigprevexpr___TOP__adc_interface_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14610722720986780716ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
