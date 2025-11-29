// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vethernet_mac_tb.h for the primary calling header

#include "Vethernet_mac_tb__pch.h"

VL_ATTR_COLD void Vethernet_mac_tb___024root___eval_static(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_static\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__ethernet_mac_tb__DOT__clk__0 
        = vlSelfRef.ethernet_mac_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__ethernet_mac_tb__DOT__rst_n__0 
        = vlSelfRef.ethernet_mac_tb__DOT__rst_n;
    vlSelfRef.__Vtrigprevexpr_h26981c9e__1 = (0U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state));
}

VL_ATTR_COLD void Vethernet_mac_tb___024root___eval_final(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_final\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vethernet_mac_tb___024root___eval_settle(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_settle\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

bool Vethernet_mac_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vethernet_mac_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vethernet_mac_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge ethernet_mac_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge ethernet_mac_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @( (4'h0 == ethernet_mac_tb.dut.tx_state))\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vethernet_mac_tb___024root___ctor_var_reset(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___ctor_var_reset\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->ethernet_mac_tb__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12064202923302927331ull);
    vlSelf->ethernet_mac_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14816933876041047ull);
    vlSelf->ethernet_mac_tb__DOT__gmii_rx_d = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17953971652510807036ull);
    vlSelf->ethernet_mac_tb__DOT__gmii_rx_dv = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3412696425849679098ull);
    vlSelf->ethernet_mac_tb__DOT__packet_data = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 2941086221903454290ull);
    vlSelf->ethernet_mac_tb__DOT__packet_len = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14221079027408820377ull);
    vlSelf->ethernet_mac_tb__DOT__packet_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2916882164740010509ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__tx_state = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2231799950225601271ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8416683147084610307ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__crc32_reg = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 15172065251188342020ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__tx_data_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4111772546945515441ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__tx_en_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12920585784200803612ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__packet_len_reg = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5346695885913197253ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 10831487371403198521ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__rx_state = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 15808906662155192565ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8117441638361699114ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 17111596589022929748ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 2370288881755297515ull);
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__link_status_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13691516377743582925ull);
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 18305763464057097933ull);
    }
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 1704021287470567676ull);
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1601524754788341852ull);
    }
    vlSelf->ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 2176176456062614797ull);
    vlSelf->__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v0 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10834299347015615420ull);
    vlSelf->__VdlySet__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v0 = 0;
    vlSelf->__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v1 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15012601163532096960ull);
    vlSelf->__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15102321518699682314ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__ethernet_mac_tb__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1784342153835929942ull);
    vlSelf->__Vtrigprevexpr___TOP__ethernet_mac_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1563875961816601451ull);
    vlSelf->__Vtrigprevexpr_h26981c9e__1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4997927552106544191ull);
    vlSelf->__VactDidInit = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
