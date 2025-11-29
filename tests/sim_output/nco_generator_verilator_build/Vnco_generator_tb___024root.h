// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vnco_generator_tb.h for the primary calling header

#ifndef VERILATED_VNCO_GENERATOR_TB___024ROOT_H_
#define VERILATED_VNCO_GENERATOR_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vnco_generator_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vnco_generator_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ nco_generator_tb__DOT__clk;
    CData/*0:0*/ nco_generator_tb__DOT__rst_n;
    CData/*0:0*/ nco_generator_tb__DOT__enable;
    CData/*0:0*/ nco_generator_tb__DOT__dut__DOT__valid_reg;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__nco_generator_tb__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__nco_generator_tb__DOT__rst_n__0;
    SData/*15:0*/ nco_generator_tb__DOT__dut__DOT__sine_reg;
    SData/*15:0*/ nco_generator_tb__DOT__dut__DOT__cosine_reg;
    IData/*31:0*/ nco_generator_tb__DOT__frequency_word;
    IData/*31:0*/ nco_generator_tb__DOT__dut__DOT__phase_accumulator;
    IData/*31:0*/ nco_generator_tb__DOT__dut__DOT__phase_accumulator_next;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<SData/*15:0*/, 256> nco_generator_tb__DOT__dut__DOT__sin_rom;
    VlUnpacked<SData/*15:0*/, 256> nco_generator_tb__DOT__dut__DOT__cos_rom;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;

    // INTERNAL VARIABLES
    Vnco_generator_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vnco_generator_tb___024root(Vnco_generator_tb__Syms* symsp, const char* v__name);
    ~Vnco_generator_tb___024root();
    VL_UNCOPYABLE(Vnco_generator_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
