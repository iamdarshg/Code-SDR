// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vudp_ip_stack_tb.h for the primary calling header

#ifndef VERILATED_VUDP_IP_STACK_TB___024ROOT_H_
#define VERILATED_VUDP_IP_STACK_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vudp_ip_stack_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vudp_ip_stack_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ udp_ip_stack_tb__DOT__clk;
    CData/*0:0*/ udp_ip_stack_tb__DOT__rst_n;
    CData/*0:0*/ udp_ip_stack_tb__DOT__app_valid;
    CData/*0:0*/ udp_ip_stack_tb__DOT__app_ready;
    CData/*2:0*/ udp_ip_stack_tb__DOT__dut__DOT__frame_state;
    CData/*4:0*/ udp_ip_stack_tb__DOT__dut__DOT__byte_counter;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__udp_ip_stack_tb__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__udp_ip_stack_tb__DOT__rst_n__0;
    SData/*15:0*/ udp_ip_stack_tb__DOT__app_len;
    SData/*15:0*/ udp_ip_stack_tb__DOT__dut__DOT__udp_length_reg;
    SData/*15:0*/ udp_ip_stack_tb__DOT__dut__DOT__total_bytes;
    SData/*15:0*/ udp_ip_stack_tb__DOT__dut__DOT__mac_len_reg;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;

    // INTERNAL VARIABLES
    Vudp_ip_stack_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vudp_ip_stack_tb___024root(Vudp_ip_stack_tb__Syms* symsp, const char* v__name);
    ~Vudp_ip_stack_tb___024root();
    VL_UNCOPYABLE(Vudp_ip_stack_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
