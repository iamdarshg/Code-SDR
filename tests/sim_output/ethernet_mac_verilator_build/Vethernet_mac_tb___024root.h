// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vethernet_mac_tb.h for the primary calling header

#ifndef VERILATED_VETHERNET_MAC_TB___024ROOT_H_
#define VERILATED_VETHERNET_MAC_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vethernet_mac_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vethernet_mac_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ ethernet_mac_tb__DOT__clk;
    CData/*0:0*/ ethernet_mac_tb__DOT__rst_n;
    CData/*7:0*/ ethernet_mac_tb__DOT__gmii_rx_d;
    CData/*0:0*/ ethernet_mac_tb__DOT__gmii_rx_dv;
    CData/*0:0*/ ethernet_mac_tb__DOT__packet_valid;
    CData/*3:0*/ ethernet_mac_tb__DOT__dut__DOT__tx_state;
    CData/*7:0*/ ethernet_mac_tb__DOT__dut__DOT__tx_byte_counter;
    CData/*7:0*/ ethernet_mac_tb__DOT__dut__DOT__tx_data_reg;
    CData/*0:0*/ ethernet_mac_tb__DOT__dut__DOT__tx_en_reg;
    CData/*3:0*/ ethernet_mac_tb__DOT__dut__DOT__rx_state;
    CData/*7:0*/ ethernet_mac_tb__DOT__dut__DOT__rx_byte_counter;
    CData/*0:0*/ ethernet_mac_tb__DOT__dut__DOT__link_status_reg;
    CData/*7:0*/ __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v0;
    CData/*0:0*/ __VdlySet__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v0;
    CData/*7:0*/ __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v1;
    CData/*7:0*/ __VdlyVal__ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev__v2;
    CData/*0:0*/ __Vtrigprevexpr___TOP__ethernet_mac_tb__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__ethernet_mac_tb__DOT__rst_n__0;
    CData/*0:0*/ __Vtrigprevexpr_h26981c9e__1;
    CData/*0:0*/ __VactDidInit;
    SData/*15:0*/ ethernet_mac_tb__DOT__packet_len;
    SData/*15:0*/ ethernet_mac_tb__DOT__dut__DOT__packet_len_reg;
    SData/*8:0*/ ethernet_mac_tb__DOT__dut__DOT__rx_buffer_write_ptr;
    IData/*31:0*/ ethernet_mac_tb__DOT__packet_data;
    IData/*31:0*/ ethernet_mac_tb__DOT__dut__DOT__crc32_reg;
    IData/*31:0*/ ethernet_mac_tb__DOT__dut__DOT__crc32_byte__Vstatic__crc_next;
    IData/*31:0*/ ethernet_mac_tb__DOT__dut__DOT__rx_crc_reg;
    IData/*31:0*/ ethernet_mac_tb__DOT__dut__DOT__rx_calculated_crc;
    IData/*31:0*/ ethernet_mac_tb__DOT__dut__DOT__packet_counter_reg;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<IData/*31:0*/, 512> ethernet_mac_tb__DOT__dut__DOT__rx_data_buffer;
    VlUnpacked<CData/*7:0*/, 3> ethernet_mac_tb__DOT__dut__DOT__gmii_rx_d_prev;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h7e9fdc72__0;

    // INTERNAL VARIABLES
    Vethernet_mac_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vethernet_mac_tb___024root(Vethernet_mac_tb__Syms* symsp, const char* v__name);
    ~Vethernet_mac_tb___024root();
    VL_UNCOPYABLE(Vethernet_mac_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
