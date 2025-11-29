// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vasync_fifo_tb.h for the primary calling header

#ifndef VERILATED_VASYNC_FIFO_TB___024ROOT_H_
#define VERILATED_VASYNC_FIFO_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vasync_fifo_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vasync_fifo_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ async_fifo_tb__DOT__wr_clk;
    CData/*0:0*/ async_fifo_tb__DOT__rd_clk;
    CData/*0:0*/ async_fifo_tb__DOT__wr_rst_n;
    CData/*0:0*/ async_fifo_tb__DOT__rd_rst_n;
    CData/*0:0*/ async_fifo_tb__DOT__wr_en;
    CData/*0:0*/ async_fifo_tb__DOT__rd_en;
    CData/*0:0*/ async_fifo_tb__DOT__test_passed;
    CData/*0:0*/ async_fifo_tb__DOT__test_completed;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__wr_ptr_gray;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__wr_ptr_bin;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_next;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__wr_ptr_bin_next;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__rd_ptr_gray;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__rd_ptr_bin;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_next;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__rd_ptr_bin_next;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_sync1;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_sync2;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_sync1;
    CData/*4:0*/ async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_sync2;
    CData/*0:0*/ async_fifo_tb__DOT__dut__DOT__full_reg;
    CData/*0:0*/ async_fifo_tb__DOT__dut__DOT__empty_reg;
    CData/*0:0*/ __Vdly__async_fifo_tb__DOT__dut__DOT__empty_reg;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__async_fifo_tb__DOT__wr_clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__async_fifo_tb__DOT__wr_rst_n__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__async_fifo_tb__DOT__rd_clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__async_fifo_tb__DOT__rd_rst_n__0;
    IData/*31:0*/ async_fifo_tb__DOT__din;
    IData/*31:0*/ async_fifo_tb__DOT__i;
    IData/*31:0*/ async_fifo_tb__DOT__expected_data;
    IData/*31:0*/ async_fifo_tb__DOT__dut__DOT__dout_reg;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<IData/*31:0*/, 16> async_fifo_tb__DOT__dut__DOT__mem;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_he0f06744__0;
    VlTriggerScheduler __VtrigSched_hd1c90b96__0;

    // INTERNAL VARIABLES
    Vasync_fifo_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vasync_fifo_tb___024root(Vasync_fifo_tb__Syms* symsp, const char* v__name);
    ~Vasync_fifo_tb___024root();
    VL_UNCOPYABLE(Vasync_fifo_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
