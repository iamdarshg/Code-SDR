// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vadc_interface_tb.h for the primary calling header

#ifndef VERILATED_VADC_INTERFACE_TB___024ROOT_H_
#define VERILATED_VADC_INTERFACE_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vadc_interface_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vadc_interface_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ adc_interface_tb__DOT__clk_adc;
    CData/*0:0*/ adc_interface_tb__DOT__rst_n;
    CData/*0:0*/ adc_interface_tb__DOT__adc_valid;
    CData/*0:0*/ adc_interface_tb__DOT__adc_ovr;
    CData/*0:0*/ adc_interface_tb__DOT__dut__DOT__adc_valid_reg;
    CData/*0:0*/ adc_interface_tb__DOT__dut__DOT__adc_ovr_reg;
    CData/*3:0*/ adc_interface_tb__DOT__dut__DOT__overflow_count;
    CData/*0:0*/ adc_interface_tb__DOT__dut__DOT__overflow_detect_reg;
    CData/*0:0*/ adc_interface_tb__DOT__dut__DOT__sample_valid_sync;
    CData/*0:0*/ __Vtrigprevexpr___TOP__adc_interface_tb__DOT__clk_adc__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__adc_interface_tb__DOT__rst_n__0;
    SData/*9:0*/ adc_interface_tb__DOT__adc_data;
    SData/*9:0*/ adc_interface_tb__DOT__dut__DOT__adc_data_reg;
    SData/*15:0*/ adc_interface_tb__DOT__dut__DOT__avg_count;
    IData/*31:0*/ adc_interface_tb__DOT__dut__DOT__dc_offset;
    IData/*31:0*/ __Vdly__adc_interface_tb__DOT__dut__DOT__dc_offset;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h6de165b8__0;

    // INTERNAL VARIABLES
    Vadc_interface_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vadc_interface_tb___024root(Vadc_interface_tb__Syms* symsp, const char* v__name);
    ~Vadc_interface_tb___024root();
    VL_UNCOPYABLE(Vadc_interface_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
