// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vaverage_power_detector_tb.h for the primary calling header

#ifndef VERILATED_VAVERAGE_POWER_DETECTOR_TB___024ROOT_H_
#define VERILATED_VAVERAGE_POWER_DETECTOR_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vaverage_power_detector_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vaverage_power_detector_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ average_power_detector_tb__DOT__clk;
    CData/*0:0*/ average_power_detector_tb__DOT__rst_n;
    CData/*0:0*/ average_power_detector_tb__DOT__sample_valid_in;
    CData/*0:0*/ average_power_detector_tb__DOT__dut__DOT__output_valid_reg;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__average_power_detector_tb__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__average_power_detector_tb__DOT__rst_n__0;
    SData/*9:0*/ average_power_detector_tb__DOT__dut__DOT__sample_count;
    IData/*31:0*/ average_power_detector_tb__DOT__sample_in;
    VlWide<3>/*73:0*/ average_power_detector_tb__DOT__dut__DOT__sum_of_squares;
    IData/*31:0*/ __VactIterCount;
    QData/*63:0*/ average_power_detector_tb__DOT__dut__DOT__sample_squared;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;

    // INTERNAL VARIABLES
    Vaverage_power_detector_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vaverage_power_detector_tb___024root(Vaverage_power_detector_tb__Syms* symsp, const char* v__name);
    ~Vaverage_power_detector_tb___024root();
    VL_UNCOPYABLE(Vaverage_power_detector_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
