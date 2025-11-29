// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vethernet_mac_tb.h for the primary calling header

#include "Vethernet_mac_tb__pch.h"

VlCoroutine Vethernet_mac_tb___024root___eval_initial__TOP__Vtiming__0(Vethernet_mac_tb___024root* vlSelf);
VlCoroutine Vethernet_mac_tb___024root___eval_initial__TOP__Vtiming__1(Vethernet_mac_tb___024root* vlSelf);

void Vethernet_mac_tb___024root___eval_initial(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_initial\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vethernet_mac_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vethernet_mac_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vethernet_mac_tb___024root___eval_initial__TOP__Vtiming__0(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.ethernet_mac_tb__DOT__clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000000fa0ULL, 
                                             nullptr, 
                                             "verilog/ethernet_mac_tb.v", 
                                             52);
        vlSelfRef.ethernet_mac_tb__DOT__clk = (1U & 
                                               (~ (IData)(vlSelfRef.ethernet_mac_tb__DOT__clk)));
    }
}

VlCoroutine Vethernet_mac_tb___024root___eval_initial__TOP__Vtiming__1(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.ethernet_mac_tb__DOT__rst_n = 0U;
    vlSelfRef.ethernet_mac_tb__DOT__packet_data = 0U;
    vlSelfRef.ethernet_mac_tb__DOT__packet_len = 0U;
    vlSelfRef.ethernet_mac_tb__DOT__packet_valid = 0U;
    vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d = 0U;
    vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/ethernet_mac_tb.v", 
                                         67);
    vlSelfRef.ethernet_mac_tb__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/ethernet_mac_tb.v", 
                                         71);
    VL_WRITEF_NX("Testing Ethernet MAC...\n",0);
    vlSelfRef.ethernet_mac_tb__DOT__packet_data = 0xdeadbeefU;
    vlSelfRef.ethernet_mac_tb__DOT__packet_len = 4U;
    vlSelfRef.ethernet_mac_tb__DOT__packet_valid = 1U;
    while ((0U != (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state))) {
        co_await vlSelfRef.__VtrigSched_h7e9fdc72__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( (4'h0 == ethernet_mac_tb.dut.tx_state))", 
                                                             "verilog/ethernet_mac_tb.v", 
                                                             81);
    }
    vlSelfRef.ethernet_mac_tb__DOT__packet_valid = 0U;
    VL_WRITEF_NX("Packet sent, waiting for transmission...\n",0);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000989680ULL, 
                                         nullptr, "verilog/ethernet_mac_tb.v", 
                                         87);
    VL_WRITEF_NX("Packet counter: %0#\n",0,32,vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg);
    vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv = 1U;
    vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d = 0x55U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000001f40ULL, 
                                         nullptr, "verilog/ethernet_mac_tb.v", 
                                         94);
    vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv = 0U;
    VL_WRITEF_NX("Link status: %b\n",0,1,vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__link_status_reg);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000f4240ULL, 
                                         nullptr, "verilog/ethernet_mac_tb.v", 
                                         99);
    VL_WRITEF_NX("Ethernet MAC test completed\n",0);
    if ((0U < vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg)) {
        VL_WRITEF_NX("\342\234\223 Packet transmission detected\n",0);
    } else {
        VL_WRITEF_NX("\342\232\240 No packets transmitted\n",0);
    }
    VL_FINISH_MT("verilog/ethernet_mac_tb.v", 110, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vethernet_mac_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vethernet_mac_tb___024root___eval_triggers__act(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_triggers__act\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vtrigprevexpr_h26981c9e__0;
    __Vtrigprevexpr_h26981c9e__0 = 0;
    // Body
    __Vtrigprevexpr_h26981c9e__0 = (0U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state));
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((((IData)(__Vtrigprevexpr_h26981c9e__0) 
                                                        != (IData)(vlSelfRef.__Vtrigprevexpr_h26981c9e__1)) 
                                                       << 3U) 
                                                      | (vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                         << 2U)) 
                                                     | ((((~ (IData)(vlSelfRef.ethernet_mac_tb__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__ethernet_mac_tb__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.ethernet_mac_tb__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__ethernet_mac_tb__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__ethernet_mac_tb__DOT__clk__0 
        = vlSelfRef.ethernet_mac_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__ethernet_mac_tb__DOT__rst_n__0 
        = vlSelfRef.ethernet_mac_tb__DOT__rst_n;
    vlSelfRef.__Vtrigprevexpr_h26981c9e__1 = __Vtrigprevexpr_h26981c9e__0;
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.__VactDidInit)))))) {
        vlSelfRef.__VactDidInit = 1U;
        vlSelfRef.__VactTriggered[0U] = (8ULL | vlSelfRef.__VactTriggered
                                         [0U]);
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vethernet_mac_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vethernet_mac_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___trigger_anySet__act\n"); );
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

void Vethernet_mac_tb___024root___nba_sequent__TOP__0(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___nba_sequent__TOP__0\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY((vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_en_reg))) {
        VL_WRITEF_NX("Time=%0t: TX Data=0x%x\n",0,64,
                     VL_TIME_UNITED_Q(1000),-9,8,(IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_data_reg));
    }
    vlSelfRef.__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v0 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev
        [1U];
    vlSelfRef.__VdlySet__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v0 = 1U;
    vlSelfRef.__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v1 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev
        [2U];
    vlSelfRef.__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v2 
        = vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d;
}

void Vethernet_mac_tb___024root___nba_sequent__TOP__1(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___nba_sequent__TOP__1\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    QData/*47:0*/ ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac;
    ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac = 0;
    QData/*47:0*/ ethernet_mac_tb__DOT__dut__DOT__rx_src_mac;
    ethernet_mac_tb__DOT__dut__DOT__rx_src_mac = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__Vfuncout;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc = 0;
    CData/*7:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__Vfuncout;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc = 0;
    CData/*7:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__Vfuncout;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc = 0;
    CData/*7:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__Vfuncout;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc = 0;
    CData/*7:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__Vfuncout;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc = 0;
    CData/*7:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__Vfuncout;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc = 0;
    CData/*7:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__Vfuncout;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc = 0;
    CData/*7:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__Vfuncout;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc = 0;
    CData/*7:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__Vfuncout;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc = 0;
    CData/*7:0*/ __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data;
    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data = 0;
    CData/*3:0*/ __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 0;
    CData/*7:0*/ __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg = 0;
    CData/*7:0*/ __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter = 0;
    IData/*31:0*/ __Vdly__ethernet_mac_tb__DOT__dut__DOT__crc32_reg;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__crc32_reg = 0;
    IData/*31:0*/ __Vdly__ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg = 0;
    CData/*3:0*/ __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 0;
    CData/*7:0*/ __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter = 0;
    IData/*31:0*/ __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc = 0;
    IData/*31:0*/ __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg = 0;
    CData/*7:0*/ __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0;
    __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0 = 0;
    SData/*8:0*/ __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0;
    __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0 = 0;
    CData/*0:0*/ __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0;
    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0 = 0;
    CData/*7:0*/ __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1;
    __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1 = 0;
    SData/*8:0*/ __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1;
    __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1 = 0;
    CData/*0:0*/ __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1;
    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1 = 0;
    CData/*7:0*/ __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2;
    __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2 = 0;
    SData/*8:0*/ __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2;
    __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2 = 0;
    CData/*0:0*/ __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2;
    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2 = 0;
    CData/*7:0*/ __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3;
    __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3 = 0;
    SData/*8:0*/ __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3;
    __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3 = 0;
    CData/*0:0*/ __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3;
    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3 = 0;
    // Body
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_data_reg;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__crc32_reg 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_reg;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_state;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc;
    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg 
        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg;
    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0 = 0U;
    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1 = 0U;
    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2 = 0U;
    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3 = 0U;
    if (vlSelfRef.ethernet_mac_tb__DOT__rst_n) {
        if (((6U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state)) 
             & (0U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter)))) {
            __Vdly__ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg 
                = ((IData)(1U) + vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg);
        }
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__link_status_reg 
            = ((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv) 
               | (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__link_status_reg));
        if ((8U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_state))) {
            __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 0U;
        } else if ((4U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_state))) {
            if ((2U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_state))) {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 0U;
            } else if ((1U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_state))) {
                if ((1U & (~ (IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv)))) {
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter 
                        = (0x000000ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter)));
                    if ((0U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc 
                            = ((0xffffff00U & __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc) 
                               | (IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d));
                    } else if ((1U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc 
                            = ((0xffff00ffU & __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc) 
                               | ((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d) 
                                  << 8U));
                    } else if ((2U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc 
                            = ((0xff00ffffU & __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc) 
                               | ((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d) 
                                  << 0x00000010U));
                    } else if ((3U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc 
                            = ((0x00ffffffU & __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc) 
                               | ((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d) 
                                  << 0x00000018U));
                    }
                    if ((3U <= (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state 
                            = ((((((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d) 
                                   == (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc 
                                       >> 0x18U)) & 
                                  (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev
                                   [0U] == (0x000000ffU 
                                            & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc 
                                               >> 0x10U)))) 
                                 & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev
                                    [1U] == (0x000000ffU 
                                             & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc 
                                                >> 8U)))) 
                                & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev
                                   [2U] == (0x000000ffU 
                                            & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc)))
                                ? 6U : 7U);
                    }
                }
            } else if (vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv) {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter 
                    = (0x000000ffU & ((IData)(1U) + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter)));
                __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data 
                    = vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d;
                if ((0U == (3U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr)))) {
                    __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0 
                        = vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d;
                    __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0 
                        = (0x0000007fU & ((IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr) 
                                          >> 2U));
                    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0 = 1U;
                } else if ((1U == (3U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr)))) {
                    __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1 
                        = vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d;
                    __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1 
                        = (0x0000007fU & ((IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr) 
                                          >> 2U));
                    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1 = 1U;
                } else if ((2U == (3U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr)))) {
                    __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2 
                        = vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d;
                    __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2 
                        = (0x0000007fU & ((IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr) 
                                          >> 2U));
                    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2 = 1U;
                } else {
                    __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3 
                        = vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d;
                    __VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3 
                        = (0x0000007fU & ((IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr) 
                                          >> 2U));
                    __VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3 = 1U;
                }
                __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc 
                    = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg;
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr 
                    = (0x000001ffU & ((IData)(1U) + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xfffffffeU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (1U & ((__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc 
                                 >> 0x18U) ^ (IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data))));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xfffffffdU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (2U & ((VL_REDXOR_32((0x03000000U 
                                               & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc)) 
                                 ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data) 
                                    >> 1U)) << 1U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xfffffffbU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (4U & ((VL_REDXOR_32((0x07000000U 
                                               & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc)) 
                                 ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data) 
                                    >> 2U)) << 2U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xfffffff7U & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (8U & ((VL_REDXOR_32((0x0f000000U 
                                               & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc)) 
                                 ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data) 
                                    >> 3U)) << 3U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xffffffefU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (0x00000010U & ((VL_REDXOR_32(
                                                       (0x1e000000U 
                                                        & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc)) 
                                          ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data) 
                                             >> 4U)) 
                                         << 4U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xffffffdfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (0x00000020U & ((VL_REDXOR_32(
                                                       (0x3c000000U 
                                                        & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc)) 
                                          ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data) 
                                             >> 5U)) 
                                         << 5U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xffffffbfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (0x00000040U & ((VL_REDXOR_32(
                                                       (0x78000000U 
                                                        & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc)) 
                                          ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data) 
                                             >> 6U)) 
                                         << 6U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xffffff7fU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (0x00000080U & ((VL_REDXOR_32(
                                                       (0xf0000000U 
                                                        & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc)) 
                                          ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__data) 
                                             >> 7U)) 
                                         << 7U)));
                __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__Vfuncout 
                    = ((0x00ff0000U & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                                       << 0x00000010U)) 
                       | (0x0000ffffU & (__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__crc 
                                         >> 8U)));
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg 
                    = __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__7__Vfuncout;
            } else {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 5U;
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter = 0U;
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc 
                    = (~ vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg);
            }
        } else if ((2U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_state))) {
            if ((1U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_state))) {
                if (vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv) {
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data 
                        = vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d;
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter 
                        = (0x000000ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc 
                        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg;
                    if (((((((((0U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter)) 
                               | (1U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) 
                              | (2U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) 
                             | (3U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) 
                            | (4U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) 
                           | (5U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) 
                          | (6U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) 
                         | (7U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter)))) {
                        if ((0U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                            ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac 
                                = ((0x000000ffffffffffULL 
                                    & ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac) 
                                   | ((QData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)) 
                                      << 0x00000028U));
                        } else if ((1U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                            ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac 
                                = ((0x0000ff00ffffffffULL 
                                    & ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac) 
                                   | ((QData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)) 
                                      << 0x00000020U));
                        } else if ((2U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                            ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac 
                                = ((0x0000ffff00ffffffULL 
                                    & ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac) 
                                   | ((QData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)) 
                                      << 0x00000018U));
                        } else if ((3U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                            ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac 
                                = ((0x0000ffffff00ffffULL 
                                    & ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac) 
                                   | ((QData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)) 
                                      << 0x00000010U));
                        } else if ((4U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                            ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac 
                                = ((0x0000ffffffff00ffULL 
                                    & ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac) 
                                   | ((QData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)) 
                                      << 8U));
                        } else if ((5U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                            ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac 
                                = ((0x0000ffffffffff00ULL 
                                    & ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac) 
                                   | (IData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)));
                        } else {
                            ethernet_mac_tb__DOT__dut__DOT__rx_src_mac 
                                = ((6U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))
                                    ? ((0x000000ffffffffffULL 
                                        & ethernet_mac_tb__DOT__dut__DOT__rx_src_mac) 
                                       | ((QData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)) 
                                          << 0x00000028U))
                                    : ((0x0000ff00ffffffffULL 
                                        & ethernet_mac_tb__DOT__dut__DOT__rx_src_mac) 
                                       | ((QData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)) 
                                          << 0x00000020U)));
                        }
                    } else if ((8U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        ethernet_mac_tb__DOT__dut__DOT__rx_src_mac 
                            = ((0x0000ffff00ffffffULL 
                                & ethernet_mac_tb__DOT__dut__DOT__rx_src_mac) 
                               | ((QData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)) 
                                  << 0x00000018U));
                    } else if ((9U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        ethernet_mac_tb__DOT__dut__DOT__rx_src_mac 
                            = ((0x0000ffffff00ffffULL 
                                & ethernet_mac_tb__DOT__dut__DOT__rx_src_mac) 
                               | ((QData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)) 
                                  << 0x00000010U));
                    } else if ((0x0aU == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        ethernet_mac_tb__DOT__dut__DOT__rx_src_mac 
                            = ((0x0000ffffffff00ffULL 
                                & ethernet_mac_tb__DOT__dut__DOT__rx_src_mac) 
                               | ((QData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)) 
                                  << 8U));
                    } else if ((0x0bU == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        ethernet_mac_tb__DOT__dut__DOT__rx_src_mac 
                            = ((0x0000ffffffffff00ULL 
                                & ethernet_mac_tb__DOT__dut__DOT__rx_src_mac) 
                               | (IData)((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)));
                    }
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffeU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (1U & ((__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc 
                                     >> 0x18U) ^ (IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data))));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffdU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (2U & ((VL_REDXOR_32((0x03000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data) 
                                        >> 1U)) << 1U)));
                    if ((0x0dU <= (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 4U;
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter = 0U;
                    }
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffbU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (4U & ((VL_REDXOR_32((0x07000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data) 
                                        >> 2U)) << 2U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffff7U & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (8U & ((VL_REDXOR_32((0x0f000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data) 
                                        >> 3U)) << 3U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffefU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000010U & ((VL_REDXOR_32(
                                                           (0x1e000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data) 
                                                 >> 4U)) 
                                             << 4U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffdfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000020U & ((VL_REDXOR_32(
                                                           (0x3c000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data) 
                                                 >> 5U)) 
                                             << 5U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffbfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000040U & ((VL_REDXOR_32(
                                                           (0x78000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data) 
                                                 >> 6U)) 
                                             << 6U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffff7fU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000080U & ((VL_REDXOR_32(
                                                           (0xf0000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__data) 
                                                 >> 7U)) 
                                             << 7U)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__Vfuncout 
                        = ((0x00ff0000U & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                                           << 0x00000010U)) 
                           | (0x0000ffffU & (__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__crc 
                                             >> 8U)));
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg 
                        = __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__8__Vfuncout;
                } else {
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 7U;
                }
            } else if (((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv) 
                        & (0xd5U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)))) {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 3U;
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter = 0U;
            } else {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state 
                    = ((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv)
                        ? 7U : 0U);
            }
        } else if ((1U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_state))) {
            if (vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv) {
                if ((0x55U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d))) {
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter 
                        = (0x000000ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter)));
                    if ((6U <= (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter))) {
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 2U;
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter = 0U;
                    }
                } else {
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 7U;
                }
            } else {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 0U;
            }
        } else {
            vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr = 0U;
            if (((IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv) 
                 & (0x55U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_d)))) {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 1U;
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter = 0U;
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg = 0xffffffffU;
            } else if ((1U & (~ (IData)(vlSelfRef.ethernet_mac_tb__DOT__gmii_rx_dv)))) {
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__link_status_reg = 0U;
            }
        }
        if ((8U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state))) {
            __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 0U;
        } else if ((4U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state))) {
            if ((2U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state))) {
                if ((1U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state))) {
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 0U;
                } else {
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_en_reg = 0U;
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg = 0U;
                    if ((0x0bU > (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))) {
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter)));
                    } else {
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 0U;
                        __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter = 0U;
                    }
                }
            } else if ((1U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state))) {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg 
                    = (0x000000ffU & ((0U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                       ? vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_reg
                                       : ((1U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                           ? (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_reg 
                                              >> 8U)
                                           : ((2U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                               ? (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_reg 
                                                  >> 0x10U)
                                               : ((3U 
                                                   == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                   ? 
                                                  (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_reg 
                                                   >> 0x18U)
                                                   : 0U)))));
                if ((4U > (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))) {
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter 
                        = (0x000000ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter)));
                } else {
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter = 0U;
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 6U;
                }
            } else if (((IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter) 
                        < (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__packet_len_reg))) {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg 
                    = (0x000000ffU & ((2U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                       ? ((1U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                           ? (vlSelfRef.ethernet_mac_tb__DOT__packet_data 
                                              >> 0x18U)
                                           : (vlSelfRef.ethernet_mac_tb__DOT__packet_data 
                                              >> 0x10U))
                                       : ((1U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                           ? (vlSelfRef.ethernet_mac_tb__DOT__packet_data 
                                              >> 8U)
                                           : vlSelfRef.ethernet_mac_tb__DOT__packet_data)));
                if ((3U == (3U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter)))) {
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data 
                        = (vlSelfRef.ethernet_mac_tb__DOT__packet_data 
                           >> 0x18U);
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data 
                        = (0x000000ffU & (vlSelfRef.ethernet_mac_tb__DOT__packet_data 
                                          >> 0x10U));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data 
                        = (0x000000ffU & (vlSelfRef.ethernet_mac_tb__DOT__packet_data 
                                          >> 8U));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data 
                        = (0x000000ffU & vlSelfRef.ethernet_mac_tb__DOT__packet_data);
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc 
                        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_reg;
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffeU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (1U & ((__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc 
                                     >> 0x18U) ^ (IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data))));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffdU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (2U & ((VL_REDXOR_32((0x03000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data) 
                                        >> 1U)) << 1U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffbU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (4U & ((VL_REDXOR_32((0x07000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data) 
                                        >> 2U)) << 2U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffff7U & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (8U & ((VL_REDXOR_32((0x0f000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data) 
                                        >> 3U)) << 3U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffefU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000010U & ((VL_REDXOR_32(
                                                           (0x1e000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data) 
                                                 >> 4U)) 
                                             << 4U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffdfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000020U & ((VL_REDXOR_32(
                                                           (0x3c000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data) 
                                                 >> 5U)) 
                                             << 5U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffbfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000040U & ((VL_REDXOR_32(
                                                           (0x78000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data) 
                                                 >> 6U)) 
                                             << 6U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffff7fU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000080U & ((VL_REDXOR_32(
                                                           (0xf0000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__data) 
                                                 >> 7U)) 
                                             << 7U)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__Vfuncout 
                        = ((0x00ff0000U & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                                           << 0x00000010U)) 
                           | (0x0000ffffU & (__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__crc 
                                             >> 8U)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc 
                        = __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__3__Vfuncout;
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffeU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (1U & ((__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc 
                                     >> 0x18U) ^ (IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data))));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffdU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (2U & ((VL_REDXOR_32((0x03000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data) 
                                        >> 1U)) << 1U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffbU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (4U & ((VL_REDXOR_32((0x07000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data) 
                                        >> 2U)) << 2U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffff7U & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (8U & ((VL_REDXOR_32((0x0f000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data) 
                                        >> 3U)) << 3U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffefU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000010U & ((VL_REDXOR_32(
                                                           (0x1e000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data) 
                                                 >> 4U)) 
                                             << 4U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffdfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000020U & ((VL_REDXOR_32(
                                                           (0x3c000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data) 
                                                 >> 5U)) 
                                             << 5U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffbfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000040U & ((VL_REDXOR_32(
                                                           (0x78000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data) 
                                                 >> 6U)) 
                                             << 6U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffff7fU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000080U & ((VL_REDXOR_32(
                                                           (0xf0000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__data) 
                                                 >> 7U)) 
                                             << 7U)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__Vfuncout 
                        = ((0x00ff0000U & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                                           << 0x00000010U)) 
                           | (0x0000ffffU & (__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__crc 
                                             >> 8U)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc 
                        = __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__2__Vfuncout;
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffeU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (1U & ((__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc 
                                     >> 0x18U) ^ (IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data))));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffdU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (2U & ((VL_REDXOR_32((0x03000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data) 
                                        >> 1U)) << 1U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffbU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (4U & ((VL_REDXOR_32((0x07000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data) 
                                        >> 2U)) << 2U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffff7U & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (8U & ((VL_REDXOR_32((0x0f000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data) 
                                        >> 3U)) << 3U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffefU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000010U & ((VL_REDXOR_32(
                                                           (0x1e000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data) 
                                                 >> 4U)) 
                                             << 4U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffdfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000020U & ((VL_REDXOR_32(
                                                           (0x3c000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data) 
                                                 >> 5U)) 
                                             << 5U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffbfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000040U & ((VL_REDXOR_32(
                                                           (0x78000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data) 
                                                 >> 6U)) 
                                             << 6U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffff7fU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000080U & ((VL_REDXOR_32(
                                                           (0xf0000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__data) 
                                                 >> 7U)) 
                                             << 7U)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__Vfuncout 
                        = ((0x00ff0000U & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                                           << 0x00000010U)) 
                           | (0x0000ffffU & (__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__crc 
                                             >> 8U)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc 
                        = __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__1__Vfuncout;
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffeU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (1U & ((__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc 
                                     >> 0x18U) ^ (IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data))));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffdU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (2U & ((VL_REDXOR_32((0x03000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data) 
                                        >> 1U)) << 1U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffbU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (4U & ((VL_REDXOR_32((0x07000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data) 
                                        >> 2U)) << 2U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffff7U & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (8U & ((VL_REDXOR_32((0x0f000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data) 
                                        >> 3U)) << 3U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffefU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000010U & ((VL_REDXOR_32(
                                                           (0x1e000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data) 
                                                 >> 4U)) 
                                             << 4U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffdfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000020U & ((VL_REDXOR_32(
                                                           (0x3c000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data) 
                                                 >> 5U)) 
                                             << 5U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffbfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000040U & ((VL_REDXOR_32(
                                                           (0x78000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data) 
                                                 >> 6U)) 
                                             << 6U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffff7fU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000080U & ((VL_REDXOR_32(
                                                           (0xf0000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__data) 
                                                 >> 7U)) 
                                             << 7U)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__Vfuncout 
                        = ((0x00ff0000U & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                                           << 0x00000010U)) 
                           | (0x0000ffffU & (__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__crc 
                                             >> 8U)));
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__crc32_reg 
                        = __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__0__Vfuncout;
                }
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter 
                    = (0x000000ffU & ((IData)(1U) + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter)));
            } else {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter = 0U;
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 5U;
            }
        } else if ((2U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state))) {
            if ((1U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state))) {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg 
                    = ((0x00000080U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                        ? 0U : ((0x00000040U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                 ? 0U : ((0x00000020U 
                                          & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                          ? 0U : ((0x00000010U 
                                                   & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                   ? 0U
                                                   : 
                                                  ((8U 
                                                    & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                    ? 
                                                   ((4U 
                                                     & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                     ? 
                                                    ((2U 
                                                      & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                      ? 0U
                                                      : 
                                                     ((1U 
                                                       & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                       ? 0U
                                                       : 8U))
                                                     : 
                                                    ((2U 
                                                      & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                      ? 
                                                     ((1U 
                                                       & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                       ? 1U
                                                       : 0U)
                                                      : 0U))
                                                    : 
                                                   ((4U 
                                                     & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                     ? 
                                                    ((2U 
                                                      & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                      ? 
                                                     ((1U 
                                                       & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))
                                                       ? 0U
                                                       : 2U)
                                                      : 0xffU)
                                                     : 0xffU))))));
                if ((0x0eU > (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))) {
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data 
                        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_data_reg;
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter 
                        = (0x000000ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc 
                        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_reg;
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffeU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (1U & ((__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc 
                                     >> 0x18U) ^ (IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data))));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffdU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (2U & ((VL_REDXOR_32((0x03000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data) 
                                        >> 1U)) << 1U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffbU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (4U & ((VL_REDXOR_32((0x07000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data) 
                                        >> 2U)) << 2U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffff7U & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (8U & ((VL_REDXOR_32((0x0f000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data) 
                                        >> 3U)) << 3U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffefU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000010U & ((VL_REDXOR_32(
                                                           (0x1e000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data) 
                                                 >> 4U)) 
                                             << 4U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffdfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000020U & ((VL_REDXOR_32(
                                                           (0x3c000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data) 
                                                 >> 5U)) 
                                             << 5U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffbfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000040U & ((VL_REDXOR_32(
                                                           (0x78000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data) 
                                                 >> 6U)) 
                                             << 6U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffff7fU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000080U & ((VL_REDXOR_32(
                                                           (0xf0000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__data) 
                                                 >> 7U)) 
                                             << 7U)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__Vfuncout 
                        = ((0x00ff0000U & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                                           << 0x00000010U)) 
                           | (0x0000ffffU & (__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__crc 
                                             >> 8U)));
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__crc32_reg 
                        = __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__4__Vfuncout;
                } else {
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter = 0U;
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 4U;
                }
            } else {
                if ((0U == (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))) {
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data 
                        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_data_reg;
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter 
                        = (0x000000ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc 
                        = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_reg;
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffeU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (1U & ((__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc 
                                     >> 0x18U) ^ (IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data))));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffdU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (2U & ((VL_REDXOR_32((0x03000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data) 
                                        >> 1U)) << 1U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffffbU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (4U & ((VL_REDXOR_32((0x07000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data) 
                                        >> 2U)) << 2U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xfffffff7U & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (8U & ((VL_REDXOR_32((0x0f000000U 
                                                   & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc)) 
                                     ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data) 
                                        >> 3U)) << 3U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffefU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000010U & ((VL_REDXOR_32(
                                                           (0x1e000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data) 
                                                 >> 4U)) 
                                             << 4U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffdfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000020U & ((VL_REDXOR_32(
                                                           (0x3c000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data) 
                                                 >> 5U)) 
                                             << 5U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffffbfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000040U & ((VL_REDXOR_32(
                                                           (0x78000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data) 
                                                 >> 6U)) 
                                             << 6U)));
                    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                        = ((0xffffff7fU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                           | (0x00000080U & ((VL_REDXOR_32(
                                                           (0xf0000000U 
                                                            & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc)) 
                                              ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__data) 
                                                 >> 7U)) 
                                             << 7U)));
                    __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__Vfuncout 
                        = ((0x00ff0000U & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                                           << 0x00000010U)) 
                           | (0x0000ffffU & (__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__crc 
                                             >> 8U)));
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__crc32_reg 
                        = __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__5__Vfuncout;
                } else {
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter = 0U;
                    __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 3U;
                }
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg = 0xd5U;
            }
        } else if ((1U & (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state))) {
            vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_en_reg = 1U;
            if ((6U > (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter))) {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter 
                    = (0x000000ffU & ((IData)(1U) + (IData)(vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter)));
                __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data 
                    = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_data_reg;
                __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc 
                    = vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_reg;
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xfffffffeU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (1U & ((__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc 
                                 >> 0x18U) ^ (IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data))));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xfffffffdU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (2U & ((VL_REDXOR_32((0x03000000U 
                                               & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc)) 
                                 ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data) 
                                    >> 1U)) << 1U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xfffffffbU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (4U & ((VL_REDXOR_32((0x07000000U 
                                               & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc)) 
                                 ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data) 
                                    >> 2U)) << 2U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xfffffff7U & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (8U & ((VL_REDXOR_32((0x0f000000U 
                                               & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc)) 
                                 ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data) 
                                    >> 3U)) << 3U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xffffffefU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (0x00000010U & ((VL_REDXOR_32(
                                                       (0x1e000000U 
                                                        & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc)) 
                                          ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data) 
                                             >> 4U)) 
                                         << 4U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xffffffdfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (0x00000020U & ((VL_REDXOR_32(
                                                       (0x3c000000U 
                                                        & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc)) 
                                          ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data) 
                                             >> 5U)) 
                                         << 5U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xffffffbfU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (0x00000040U & ((VL_REDXOR_32(
                                                       (0x78000000U 
                                                        & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc)) 
                                          ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data) 
                                             >> 6U)) 
                                         << 6U)));
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                    = ((0xffffff7fU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next) 
                       | (0x00000080U & ((VL_REDXOR_32(
                                                       (0xf0000000U 
                                                        & __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc)) 
                                          ^ ((IData)(__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__data) 
                                             >> 7U)) 
                                         << 7U)));
                __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__Vfuncout 
                    = ((0x00ff0000U & (vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next 
                                       << 0x00000010U)) 
                       | (0x0000ffffU & (__Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__crc 
                                         >> 8U)));
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__crc32_reg 
                    = __Vfunc_ethernet_mac_tb__DOT__dut__DOT__crc32_byte__6__Vfuncout;
            } else {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter = 0U;
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 2U;
            }
            __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg = 0x55U;
        } else {
            if (vlSelfRef.ethernet_mac_tb__DOT__packet_valid) {
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter = 0U;
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg 
                    = ((IData)(1U) + vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg);
                vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__packet_len_reg 
                    = vlSelfRef.ethernet_mac_tb__DOT__packet_len;
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__crc32_reg = 0xffffffffU;
                __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 1U;
            }
            vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_en_reg = 0U;
        }
    } else {
        __Vdly__ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg = 0U;
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__link_status_reg = 0U;
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr = 0U;
        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state = 0U;
        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter = 0U;
        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg = 0xffffffffU;
        __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc = 0U;
        ethernet_mac_tb__DOT__dut__DOT__rx_dest_mac = 0ULL;
        ethernet_mac_tb__DOT__dut__DOT__rx_src_mac = 0ULL;
        __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter = 0U;
        __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state = 0U;
        __Vdly__ethernet_mac_tb__DOT__dut__DOT__crc32_reg = 0xffffffffU;
        __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg = 0U;
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_en_reg = 0U;
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__packet_len_reg = 0U;
    }
    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_state 
        = __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_state;
    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter 
        = __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter;
    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc 
        = __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc;
    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg 
        = __Vdly__ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg;
    if (__VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0) {
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer[__VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0] 
            = ((0xffffff00U & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer
                [__VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0]) 
               | (IData)(__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v0));
    }
    if (__VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1) {
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer[__VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1] 
            = ((0xffff00ffU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer
                [__VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1]) 
               | ((IData)(__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v1) 
                  << 8U));
    }
    if (__VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2) {
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer[__VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2] 
            = ((0xff00ffffU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer
                [__VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2]) 
               | ((IData)(__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v2) 
                  << 0x00000010U));
    }
    if (__VdlySet__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3) {
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer[__VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3] 
            = ((0x00ffffffU & vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer
                [__VdlyDim0__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3]) 
               | ((IData)(__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer__v3) 
                  << 0x00000018U));
    }
    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_state 
        = __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_state;
    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter 
        = __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter;
    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__crc32_reg 
        = __Vdly__ethernet_mac_tb__DOT__dut__DOT__crc32_reg;
    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__tx_data_reg 
        = __Vdly__ethernet_mac_tb__DOT__dut__DOT__tx_data_reg;
    vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg 
        = __Vdly__ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg;
}

void Vethernet_mac_tb___024root___nba_sequent__TOP__2(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___nba_sequent__TOP__2\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.__VdlySet__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v0) {
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev[0U] 
            = vlSelfRef.__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v0;
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev[1U] 
            = vlSelfRef.__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v1;
        vlSelfRef.ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev[2U] 
            = vlSelfRef.__VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v2;
    }
}

void Vethernet_mac_tb___024root___eval_nba(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_nba\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vethernet_mac_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vethernet_mac_tb___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vethernet_mac_tb___024root___nba_sequent__TOP__2(vlSelf);
    }
}

void Vethernet_mac_tb___024root___timing_commit(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___timing_commit\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((! (8ULL & vlSelfRef.__VactTriggered[0U]))) {
        vlSelfRef.__VtrigSched_h7e9fdc72__0.commit(
                                                   "@( (4'h0 == ethernet_mac_tb.dut.tx_state))");
    }
}

void Vethernet_mac_tb___024root___timing_resume(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___timing_resume\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((8ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h7e9fdc72__0.resume(
                                                   "@( (4'h0 == ethernet_mac_tb.dut.tx_state))");
    }
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vethernet_mac_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vethernet_mac_tb___024root___eval_phase__act(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_phase__act\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vethernet_mac_tb___024root___eval_triggers__act(vlSelf);
    Vethernet_mac_tb___024root___timing_commit(vlSelf);
    Vethernet_mac_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vethernet_mac_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vethernet_mac_tb___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

void Vethernet_mac_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vethernet_mac_tb___024root___eval_phase__nba(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_phase__nba\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vethernet_mac_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vethernet_mac_tb___024root___eval_nba(vlSelf);
        Vethernet_mac_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vethernet_mac_tb___024root___eval(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vethernet_mac_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/ethernet_mac_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vethernet_mac_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/ethernet_mac_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vethernet_mac_tb___024root___eval_phase__act(vlSelf));
    } while (Vethernet_mac_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vethernet_mac_tb___024root___eval_debug_assertions(Vethernet_mac_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vethernet_mac_tb___024root___eval_debug_assertions\n"); );
    Vethernet_mac_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
