// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vrp2040_interface_tb.h for the primary calling header

#ifndef VERILATED_VRP2040_INTERFACE_TB___024ROOT_H_
#define VERILATED_VRP2040_INTERFACE_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vrp2040_interface_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vrp2040_interface_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ rp2040_interface_tb__DOT__spi_clk;
    CData/*0:0*/ rp2040_interface_tb__DOT__spi_mosi;
    CData/*0:0*/ rp2040_interface_tb__DOT__spi_cs_n;
    CData/*1:0*/ rp2040_interface_tb__DOT__dut__DOT__spi_state;
    CData/*7:0*/ rp2040_interface_tb__DOT__dut__DOT__spi_addr;
    CData/*3:0*/ rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter;
    CData/*7:0*/ rp2040_interface_tb__DOT__dut__DOT__gain_ctrl_reg;
    CData/*2:0*/ rp2040_interface_tb__DOT__dut__DOT__proc_mode_reg;
    CData/*7:0*/ rp2040_interface_tb__DOT__dut__DOT__demod_type_reg;
    CData/*0:0*/ rp2040_interface_tb__DOT__dut__DOT__clk_gating_reg;
    CData/*0:0*/ __Vtrigprevexpr___TOP__rp2040_interface_tb__DOT__spi_clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__rp2040_interface_tb__DOT__spi_cs_n__0;
    SData/*15:0*/ rp2040_interface_tb__DOT__status_reg;
    IData/*31:0*/ rp2040_interface_tb__DOT__dut__DOT__spi_data_in;
    IData/*31:0*/ rp2040_interface_tb__DOT__dut__DOT__spi_data_out;
    IData/*31:0*/ rp2040_interface_tb__DOT__dut__DOT__freq_word_reg;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;

    // INTERNAL VARIABLES
    Vrp2040_interface_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vrp2040_interface_tb___024root(Vrp2040_interface_tb__Syms* symsp, const char* v__name);
    ~Vrp2040_interface_tb___024root();
    VL_UNCOPYABLE(Vrp2040_interface_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
