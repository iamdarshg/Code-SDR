// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vrp2040_interface_tb.h for the primary calling header

#include "Vrp2040_interface_tb__pch.h"

VL_ATTR_COLD void Vrp2040_interface_tb___024root___eval_static(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_static\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__rp2040_interface_tb__DOT__spi_clk__0 
        = vlSelfRef.rp2040_interface_tb__DOT__spi_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rp2040_interface_tb__DOT__spi_cs_n__0 
        = vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n;
}

VL_ATTR_COLD void Vrp2040_interface_tb___024root___eval_final(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_final\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vrp2040_interface_tb___024root___eval_settle(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_settle\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

bool Vrp2040_interface_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vrp2040_interface_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vrp2040_interface_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge rp2040_interface_tb.spi_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge rp2040_interface_tb.spi_cs_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vrp2040_interface_tb___024root___ctor_var_reset(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___ctor_var_reset\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->rp2040_interface_tb__DOT__spi_clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3126383339845735804ull);
    vlSelf->rp2040_interface_tb__DOT__spi_mosi = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14301769064099224200ull);
    vlSelf->rp2040_interface_tb__DOT__spi_cs_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11683416617982194938ull);
    vlSelf->rp2040_interface_tb__DOT__status_reg = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4019263470792033309ull);
    vlSelf->rp2040_interface_tb__DOT__dut__DOT__spi_state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 8710098388242915180ull);
    vlSelf->rp2040_interface_tb__DOT__dut__DOT__spi_addr = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15982194598954201555ull);
    vlSelf->rp2040_interface_tb__DOT__dut__DOT__spi_data_in = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 16072950133963386790ull);
    vlSelf->rp2040_interface_tb__DOT__dut__DOT__spi_data_out = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4176077583655822847ull);
    vlSelf->rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 17842055111521238565ull);
    vlSelf->rp2040_interface_tb__DOT__dut__DOT__freq_word_reg = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 3467160336827928479ull);
    vlSelf->rp2040_interface_tb__DOT__dut__DOT__gain_ctrl_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8391248453603673232ull);
    vlSelf->rp2040_interface_tb__DOT__dut__DOT__proc_mode_reg = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 2689527713515002469ull);
    vlSelf->rp2040_interface_tb__DOT__dut__DOT__demod_type_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10332103636587525024ull);
    vlSelf->rp2040_interface_tb__DOT__dut__DOT__clk_gating_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8155215602812597823ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__rp2040_interface_tb__DOT__spi_clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17172216265456171541ull);
    vlSelf->__Vtrigprevexpr___TOP__rp2040_interface_tb__DOT__spi_cs_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11462562237052992429ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
