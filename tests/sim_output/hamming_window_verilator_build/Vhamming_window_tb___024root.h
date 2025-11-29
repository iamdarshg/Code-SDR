// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vhamming_window_tb.h for the primary calling header

#ifndef VERILATED_VHAMMING_WINDOW_TB___024ROOT_H_
#define VERILATED_VHAMMING_WINDOW_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vhamming_window_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vhamming_window_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ hamming_window_tb__DOT__clk;
    CData/*0:0*/ hamming_window_tb__DOT__rst_n;
    CData/*0:0*/ hamming_window_tb__DOT__data_valid;
    CData/*0:0*/ hamming_window_tb__DOT__dut__DOT__buffer_full;
    CData/*0:0*/ hamming_window_tb__DOT__dut__DOT__output_valid_reg;
    CData/*2:0*/ hamming_window_tb__DOT__dut__DOT__output_delay;
    CData/*2:0*/ __Vdly__hamming_window_tb__DOT__dut__DOT__output_delay;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__hamming_window_tb__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__hamming_window_tb__DOT__rst_n__0;
    SData/*9:0*/ hamming_window_tb__DOT__dut__DOT__sample_counter;
    SData/*15:0*/ hamming_window_tb__DOT__dut__DOT__current_coeff;
    SData/*9:0*/ __Vdly__hamming_window_tb__DOT__dut__DOT__sample_counter;
    IData/*31:0*/ hamming_window_tb__DOT__data_in;
    IData/*31:0*/ hamming_window_tb__DOT__output_count;
    IData/*31:0*/ hamming_window_tb__DOT__dut__DOT__windowed_data;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<SData/*15:0*/, 64> hamming_window_tb__DOT__dut__DOT__window_coeff;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h2ecb2d09__0;

    // INTERNAL VARIABLES
    Vhamming_window_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vhamming_window_tb___024root(Vhamming_window_tb__Syms* symsp, const char* v__name);
    ~Vhamming_window_tb___024root();
    VL_UNCOPYABLE(Vhamming_window_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
