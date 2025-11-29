// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vadaptive_gain_scaler_tb.h for the primary calling header

#ifndef VERILATED_VADAPTIVE_GAIN_SCALER_TB___024ROOT_H_
#define VERILATED_VADAPTIVE_GAIN_SCALER_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vadaptive_gain_scaler_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vadaptive_gain_scaler_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ adaptive_gain_scaler_tb__DOT__clk;
    CData/*0:0*/ adaptive_gain_scaler_tb__DOT__rst_n;
    CData/*0:0*/ adaptive_gain_scaler_tb__DOT__sample_valid_in;
    CData/*7:0*/ adaptive_gain_scaler_tb__DOT__gain_control;
    CData/*0:0*/ adaptive_gain_scaler_tb__DOT__dut__DOT__sample_valid_out_reg;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__adaptive_gain_scaler_tb__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__adaptive_gain_scaler_tb__DOT__rst_n__0;
    IData/*31:0*/ adaptive_gain_scaler_tb__DOT__sample_in;
    IData/*31:0*/ adaptive_gain_scaler_tb__DOT__sample_out;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;

    // INTERNAL VARIABLES
    Vadaptive_gain_scaler_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vadaptive_gain_scaler_tb___024root(Vadaptive_gain_scaler_tb__Syms* symsp, const char* v__name);
    ~Vadaptive_gain_scaler_tb___024root();
    VL_UNCOPYABLE(Vadaptive_gain_scaler_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
