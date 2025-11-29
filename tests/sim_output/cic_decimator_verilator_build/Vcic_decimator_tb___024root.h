// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vcic_decimator_tb.h for the primary calling header

#ifndef VERILATED_VCIC_DECIMATOR_TB___024ROOT_H_
#define VERILATED_VCIC_DECIMATOR_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vcic_decimator_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vcic_decimator_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ cic_decimator_tb__DOT__clk;
    CData/*0:0*/ cic_decimator_tb__DOT__rst_n;
    CData/*0:0*/ cic_decimator_tb__DOT__data_valid;
    CData/*0:0*/ cic_decimator_tb__DOT__test_passed;
    CData/*0:0*/ cic_decimator_tb__DOT__test_completed;
    CData/*0:0*/ cic_decimator_tb__DOT__dut__DOT__decimate_enable;
    CData/*3:0*/ cic_decimator_tb__DOT__dut__DOT__decimate_counter;
    CData/*3:0*/ __Vdly__cic_decimator_tb__DOT__dut__DOT__decimate_counter;
    CData/*0:0*/ __VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v0;
    CData/*0:0*/ __VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v1;
    CData/*0:0*/ __VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v2;
    CData/*0:0*/ __VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v3;
    CData/*0:0*/ __VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v4;
    CData/*0:0*/ __VdlySet__cic_decimator_tb__DOT__dut__DOT__comb__v5;
    CData/*0:0*/ __Vtrigprevexpr___TOP__cic_decimator_tb__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__cic_decimator_tb__DOT__rst_n__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__cic_decimator_tb__DOT__dut__DOT__decimate_enable__0;
    CData/*0:0*/ __VactDidInit;
    IData/*31:0*/ cic_decimator_tb__DOT__data_in;
    IData/*31:0*/ cic_decimator_tb__DOT__i;
    IData/*31:0*/ cic_decimator_tb__DOT__dut__DOT__comb_loop_j;
    IData/*31:0*/ cic_decimator_tb__DOT__dut__DOT__output_register;
    IData/*31:0*/ __VactIterCount;
    QData/*40:0*/ __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v0;
    QData/*40:0*/ __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v2;
    QData/*40:0*/ __VdlyVal__cic_decimator_tb__DOT__dut__DOT__comb__v4;
    VlUnpacked<QData/*40:0*/, 3> cic_decimator_tb__DOT__dut__DOT__comb;
    VlUnpacked<VlUnpacked<QData/*40:0*/, 2>, 3> cic_decimator_tb__DOT__dut__DOT__comb_delay;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlNBACommitQueue<VlUnpacked<VlUnpacked<QData/*40:0*/, 2>, 3>, false, QData/*40:0*/, 2> __VdlyCommitQueuecic_decimator_tb__DOT__dut__DOT__comb_delay;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h4b095651__0;

    // INTERNAL VARIABLES
    Vcic_decimator_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vcic_decimator_tb___024root(Vcic_decimator_tb__Syms* symsp, const char* v__name);
    ~Vcic_decimator_tb___024root();
    VL_UNCOPYABLE(Vcic_decimator_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
