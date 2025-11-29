// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vasync_fifo_tb.h for the primary calling header

#include "Vasync_fifo_tb__pch.h"

VlCoroutine Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__0(Vasync_fifo_tb___024root* vlSelf);
VlCoroutine Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__1(Vasync_fifo_tb___024root* vlSelf);
VlCoroutine Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__2(Vasync_fifo_tb___024root* vlSelf);
VlCoroutine Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__3(Vasync_fifo_tb___024root* vlSelf);

void Vasync_fifo_tb___024root___eval_initial(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_initial\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
    Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__2(vlSelf);
    Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__3(vlSelf);
}

VlCoroutine Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__0(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.async_fifo_tb__DOT__wr_clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001770ULL, 
                                             nullptr, 
                                             "verilog/async_fifo_tb.v", 
                                             41);
        vlSelfRef.async_fifo_tb__DOT__wr_clk = (1U 
                                                & (~ (IData)(vlSelfRef.async_fifo_tb__DOT__wr_clk)));
    }
}

VlCoroutine Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__1(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.async_fifo_tb__DOT__rd_clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "verilog/async_fifo_tb.v", 
                                             46);
        vlSelfRef.async_fifo_tb__DOT__rd_clk = (1U 
                                                & (~ (IData)(vlSelfRef.async_fifo_tb__DOT__rd_clk)));
    }
}

VlCoroutine Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__2(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__2\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x0000e35fa931a000ULL, 
                                         nullptr, "verilog/async_fifo_tb.v", 
                                         56);
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.async_fifo_tb__DOT__test_completed)))))) {
        VL_WRITEF_NX("TIMEOUT: Test did not complete within 250 seconds\nTEST FAILED: Timeout occurred during FIFO testing\n[ASYNC_FIFO_TB] Dumping timeout state:\n[ASYNC_FIFO_TB]   Simulation time: %0t ns\n[ASYNC_FIFO_TB]   wr_rst_n=%b, rd_rst_n=%b\n[ASYNC_FIFO_TB]   wr_en=%b, rd_en=%b\n[ASYNC_FIFO_TB]   din=%x, dout=%x\n[ASYNC_FIFO_TB]   full=%b, empty=%b\n[ASYNC_FIFO_TB]   Loop counter i=%0d\n[ASYNC_FIFO_TB]   Expected data=%x\n[ASYNC_FIFO_TB]   Test state: test_passed=%b\n[ASYNC_FIFO_TB]   FIFO internal state dump:\n",0,
                     64,VL_TIME_UNITED_Q(1000),-9,1,
                     (IData)(vlSelfRef.async_fifo_tb__DOT__wr_rst_n),
                     1,vlSelfRef.async_fifo_tb__DOT__rd_rst_n,
                     1,(IData)(vlSelfRef.async_fifo_tb__DOT__wr_en),
                     1,vlSelfRef.async_fifo_tb__DOT__rd_en,
                     32,vlSelfRef.async_fifo_tb__DOT__din,
                     32,vlSelfRef.async_fifo_tb__DOT__dut__DOT__dout_reg,
                     1,(IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__full_reg),
                     1,vlSelfRef.async_fifo_tb__DOT__dut__DOT__empty_reg,
                     32,vlSelfRef.async_fifo_tb__DOT__i,
                     32,vlSelfRef.async_fifo_tb__DOT__expected_data,
                     1,(IData)(vlSelfRef.async_fifo_tb__DOT__test_passed));
        VL_FINISH_MT("verilog/async_fifo_tb.v", 72, "");
    }
}

VlCoroutine Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__3(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_initial__TOP__Vtiming__3\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    VL_WRITEF_NX("[ASYNC_FIFO_TB] Starting FIFO testbench simulation\n[ASYNC_FIFO_TB] Test parameters: WIDTH=32, DEPTH=16\n[ASYNC_FIFO_TB] Write clock: ~83.33 MHz, Read clock: 100 MHz\n",0);
    vlSelfRef.async_fifo_tb__DOT__wr_rst_n = 0U;
    vlSelfRef.async_fifo_tb__DOT__rd_rst_n = 0U;
    vlSelfRef.async_fifo_tb__DOT__din = 0U;
    vlSelfRef.async_fifo_tb__DOT__wr_en = 0U;
    vlSelfRef.async_fifo_tb__DOT__rd_en = 0U;
    vlSelfRef.async_fifo_tb__DOT__test_passed = 1U;
    VL_WRITEF_NX("[ASYNC_FIFO_TB] Applying reset...\n",0);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/async_fifo_tb.v", 
                                         92);
    vlSelfRef.async_fifo_tb__DOT__wr_rst_n = 1U;
    vlSelfRef.async_fifo_tb__DOT__rd_rst_n = 1U;
    VL_WRITEF_NX("[ASYNC_FIFO_TB] Reset complete. Waiting for stabilization...\n",0);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/async_fifo_tb.v", 
                                         99);
    VL_WRITEF_NX("[ASYNC_FIFO_TB] Starting write operations...\n",0);
    vlSelfRef.async_fifo_tb__DOT__i = 0U;
    while (VL_GTS_III(32, 8U, vlSelfRef.async_fifo_tb__DOT__i)) {
        co_await vlSelfRef.__VtrigSched_he0f06744__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge async_fifo_tb.wr_clk)", 
                                                             "verilog/async_fifo_tb.v", 
                                                             105);
        VL_WRITEF_NX("[ASYNC_FIFO_TB] Write %0d: full=%b, din=%x\n",0,
                     32,vlSelfRef.async_fifo_tb__DOT__i,
                     1,(IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__full_reg),
                     32,vlSelfRef.async_fifo_tb__DOT__i);
        if (vlSelfRef.async_fifo_tb__DOT__dut__DOT__full_reg) {
            VL_WRITEF_NX("[ASYNC_FIFO_TB]   FIFO full, cannot write\n",0);
            vlSelfRef.async_fifo_tb__DOT__i = (vlSelfRef.async_fifo_tb__DOT__i 
                                               - (IData)(1U));
        } else {
            vlSelfRef.async_fifo_tb__DOT__din = vlSelfRef.async_fifo_tb__DOT__i;
            vlSelfRef.async_fifo_tb__DOT__wr_en = 1U;
            VL_WRITEF_NX("[ASYNC_FIFO_TB]   Writing data %x\n",0,
                         32,vlSelfRef.async_fifo_tb__DOT__i);
        }
        co_await vlSelfRef.__VtrigSched_he0f06744__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge async_fifo_tb.wr_clk)", 
                                                             "verilog/async_fifo_tb.v", 
                                                             115);
        vlSelfRef.async_fifo_tb__DOT__wr_en = 0U;
        vlSelfRef.async_fifo_tb__DOT__i = ((IData)(1U) 
                                           + vlSelfRef.async_fifo_tb__DOT__i);
    }
    VL_WRITEF_NX("[ASYNC_FIFO_TB] Write operations complete. Waiting for cross-domain sync...\n",0);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000030d40ULL, 
                                         nullptr, "verilog/async_fifo_tb.v", 
                                         122);
    VL_WRITEF_NX("[ASYNC_FIFO_TB] Starting read operations and verification...\n",0);
    vlSelfRef.async_fifo_tb__DOT__expected_data = 0U;
    vlSelfRef.async_fifo_tb__DOT__i = 0U;
    while (VL_GTS_III(32, 8U, vlSelfRef.async_fifo_tb__DOT__i)) {
        co_await vlSelfRef.__VtrigSched_hd1c90b96__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge async_fifo_tb.rd_clk)", 
                                                             "verilog/async_fifo_tb.v", 
                                                             129);
        VL_WRITEF_NX("[ASYNC_FIFO_TB] Read %0d: empty=%b, expected=%x\n",0,
                     32,vlSelfRef.async_fifo_tb__DOT__i,
                     1,(IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__empty_reg),
                     32,vlSelfRef.async_fifo_tb__DOT__expected_data);
        if (vlSelfRef.async_fifo_tb__DOT__dut__DOT__empty_reg) {
            VL_WRITEF_NX("[ASYNC_FIFO_TB]   FIFO empty, cannot read\n",0);
            vlSelfRef.async_fifo_tb__DOT__i = (vlSelfRef.async_fifo_tb__DOT__i 
                                               - (IData)(1U));
        } else {
            vlSelfRef.async_fifo_tb__DOT__rd_en = 1U;
            co_await vlSelfRef.__VtrigSched_hd1c90b96__0.trigger(0U, 
                                                                 nullptr, 
                                                                 "@(posedge async_fifo_tb.rd_clk)", 
                                                                 "verilog/async_fifo_tb.v", 
                                                                 133);
            vlSelfRef.async_fifo_tb__DOT__rd_en = 0U;
            co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                                 nullptr, 
                                                 "verilog/async_fifo_tb.v", 
                                                 135);
            VL_WRITEF_NX("[ASYNC_FIFO_TB]   Read data: %x\n",0,
                         32,vlSelfRef.async_fifo_tb__DOT__dut__DOT__dout_reg);
            if ((vlSelfRef.async_fifo_tb__DOT__dut__DOT__dout_reg 
                 != vlSelfRef.async_fifo_tb__DOT__expected_data)) {
                VL_WRITEF_NX("[ASYNC_FIFO_TB]   ERROR: DATA MISMATCH - expected %x, got %x\n",0,
                             32,vlSelfRef.async_fifo_tb__DOT__expected_data,
                             32,vlSelfRef.async_fifo_tb__DOT__dut__DOT__dout_reg);
                vlSelfRef.async_fifo_tb__DOT__test_passed = 0U;
            } else {
                VL_WRITEF_NX("[ASYNC_FIFO_TB]   Data verified successfully\n",0);
            }
            vlSelfRef.async_fifo_tb__DOT__expected_data 
                = ((IData)(1U) + vlSelfRef.async_fifo_tb__DOT__expected_data);
        }
        vlSelfRef.async_fifo_tb__DOT__i = ((IData)(1U) 
                                           + vlSelfRef.async_fifo_tb__DOT__i);
    }
    VL_WRITEF_NX("[ASYNC_FIFO_TB] Read and verification complete.\n[ASYNC_FIFO_TB] Final test state: full=%b, empty=%b\n",0,
                 1,vlSelfRef.async_fifo_tb__DOT__dut__DOT__full_reg,
                 1,(IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__empty_reg));
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/async_fifo_tb.v", 
                                         154);
    VL_WRITEF_NX("[ASYNC_FIFO_TB] Test result: %s\n",0,
                 48,((IData)(vlSelfRef.async_fifo_tb__DOT__test_passed)
                      ? 0x0000504153534544ULL : 0x00004641494c4544ULL));
    if (vlSelfRef.async_fifo_tb__DOT__test_passed) {
        VL_WRITEF_NX("TEST PASSED: FIFO read/write test successful\n",0);
    } else {
        VL_WRITEF_NX("TEST FAILED: FIFO test failed\n",0);
    }
    VL_WRITEF_NX("[ASYNC_FIFO_TB] Dumping final testbench state:\n[ASYNC_FIFO_TB]   wr_rst_n=%b, rd_rst_n=%b\n[ASYNC_FIFO_TB]   wr_en=%b, rd_en=%b\n[ASYNC_FIFO_TB]   din=%x, dout=%x\n[ASYNC_FIFO_TB]   full=%b, empty=%b\n[ASYNC_FIFO_TB]   Loop counter i=%0d\n[ASYNC_FIFO_TB]   Expected data=%x\n",0,
                 1,vlSelfRef.async_fifo_tb__DOT__wr_rst_n,
                 1,(IData)(vlSelfRef.async_fifo_tb__DOT__rd_rst_n),
                 1,vlSelfRef.async_fifo_tb__DOT__wr_en,
                 1,(IData)(vlSelfRef.async_fifo_tb__DOT__rd_en),
                 32,vlSelfRef.async_fifo_tb__DOT__din,
                 32,vlSelfRef.async_fifo_tb__DOT__dut__DOT__dout_reg,
                 1,(IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__full_reg),
                 1,vlSelfRef.async_fifo_tb__DOT__dut__DOT__empty_reg,
                 32,vlSelfRef.async_fifo_tb__DOT__i,
                 32,vlSelfRef.async_fifo_tb__DOT__expected_data);
    vlSelfRef.async_fifo_tb__DOT__test_completed = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/async_fifo_tb.v", 
                                         172);
    VL_FINISH_MT("verilog/async_fifo_tb.v", 173, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vasync_fifo_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vasync_fifo_tb___024root___eval_triggers__act(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_triggers__act\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 4U) 
                                                     | (((((~ (IData)(vlSelfRef.async_fifo_tb__DOT__rd_rst_n)) 
                                                           & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__rd_rst_n__0)) 
                                                          << 3U) 
                                                         | (((IData)(vlSelfRef.async_fifo_tb__DOT__rd_clk) 
                                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__rd_clk__0))) 
                                                            << 2U)) 
                                                        | ((((~ (IData)(vlSelfRef.async_fifo_tb__DOT__wr_rst_n)) 
                                                             & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__wr_rst_n__0)) 
                                                            << 1U) 
                                                           | ((IData)(vlSelfRef.async_fifo_tb__DOT__wr_clk) 
                                                              & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__wr_clk__0))))))));
    vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__wr_clk__0 
        = vlSelfRef.async_fifo_tb__DOT__wr_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__wr_rst_n__0 
        = vlSelfRef.async_fifo_tb__DOT__wr_rst_n;
    vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__rd_clk__0 
        = vlSelfRef.async_fifo_tb__DOT__rd_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__rd_rst_n__0 
        = vlSelfRef.async_fifo_tb__DOT__rd_rst_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vasync_fifo_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vasync_fifo_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vasync_fifo_tb___024root___nba_sequent__TOP__0(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___nba_sequent__TOP__0\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__async_fifo_tb__DOT__dut__DOT__empty_reg 
        = vlSelfRef.async_fifo_tb__DOT__dut__DOT__empty_reg;
    vlSelfRef.__Vdly__async_fifo_tb__DOT__dut__DOT__empty_reg 
        = ((1U & (~ (IData)(vlSelfRef.async_fifo_tb__DOT__rd_rst_n))) 
           || ((IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray) 
               == (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_sync2)));
    if (vlSelfRef.async_fifo_tb__DOT__rd_rst_n) {
        if (((IData)(vlSelfRef.async_fifo_tb__DOT__rd_en) 
             & (~ (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__empty_reg)))) {
            vlSelfRef.async_fifo_tb__DOT__dut__DOT__dout_reg 
                = vlSelfRef.async_fifo_tb__DOT__dut__DOT__mem
                [(0x0000000fU & (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_bin))];
            vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_bin 
                = vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_bin_next;
        }
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_sync2 
            = vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_sync1;
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_sync1 
            = vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray;
    } else {
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__dout_reg = 0U;
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_sync2 = 0U;
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_bin = 0U;
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_sync1 = 0U;
    }
    vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_bin_next 
        = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_bin)));
}

void Vasync_fifo_tb___024root___nba_sequent__TOP__1(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___nba_sequent__TOP__1\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VdlyVal__async_fifo_tb__DOT__dut__DOT__mem__v0;
    __VdlyVal__async_fifo_tb__DOT__dut__DOT__mem__v0 = 0;
    CData/*3:0*/ __VdlyDim0__async_fifo_tb__DOT__dut__DOT__mem__v0;
    __VdlyDim0__async_fifo_tb__DOT__dut__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__async_fifo_tb__DOT__dut__DOT__mem__v0;
    __VdlySet__async_fifo_tb__DOT__dut__DOT__mem__v0 = 0;
    // Body
    __VdlySet__async_fifo_tb__DOT__dut__DOT__mem__v0 = 0U;
    if (((IData)(vlSelfRef.async_fifo_tb__DOT__wr_en) 
         & (~ (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__full_reg)))) {
        __VdlyVal__async_fifo_tb__DOT__dut__DOT__mem__v0 
            = vlSelfRef.async_fifo_tb__DOT__din;
        __VdlyDim0__async_fifo_tb__DOT__dut__DOT__mem__v0 
            = (0x0000000fU & (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_bin));
        __VdlySet__async_fifo_tb__DOT__dut__DOT__mem__v0 = 1U;
    }
    if (__VdlySet__async_fifo_tb__DOT__dut__DOT__mem__v0) {
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__mem[__VdlyDim0__async_fifo_tb__DOT__dut__DOT__mem__v0] 
            = __VdlyVal__async_fifo_tb__DOT__dut__DOT__mem__v0;
    }
}

void Vasync_fifo_tb___024root___nba_sequent__TOP__2(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___nba_sequent__TOP__2\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*4:0*/ __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__Vfuncout;
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__Vfuncout = 0;
    CData/*4:0*/ __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__bin;
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__bin = 0;
    // Body
    if (vlSelfRef.async_fifo_tb__DOT__wr_rst_n) {
        if (((IData)(vlSelfRef.async_fifo_tb__DOT__wr_en) 
             & (~ (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__full_reg)))) {
            vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray 
                = vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_next;
            vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_bin 
                = vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_bin_next;
        }
    } else {
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray = 0U;
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_bin = 0U;
    }
    vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_bin_next 
        = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_bin)));
    vlSelfRef.async_fifo_tb__DOT__dut__DOT__full_reg 
        = ((IData)(vlSelfRef.async_fifo_tb__DOT__wr_rst_n) 
           && ((IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_sync2) 
               == (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_next)));
    if (vlSelfRef.async_fifo_tb__DOT__wr_rst_n) {
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_sync2 
            = vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_sync1;
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_sync1 
            = vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray;
    } else {
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_sync2 = 0U;
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_sync1 = 0U;
    }
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__bin 
        = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_bin)));
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__Vfuncout 
        = (0x0000001fU & ((IData)(__Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__bin) 
                          ^ VL_SHIFTR_III(5,5,32, (IData)(__Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__bin), 1U)));
    vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_next 
        = __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__Vfuncout;
}

void Vasync_fifo_tb___024root___nba_sequent__TOP__3(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___nba_sequent__TOP__3\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*4:0*/ __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__Vfuncout;
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__Vfuncout = 0;
    CData/*4:0*/ __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__bin;
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__bin = 0;
    // Body
    if (vlSelfRef.async_fifo_tb__DOT__rd_rst_n) {
        if (((IData)(vlSelfRef.async_fifo_tb__DOT__rd_en) 
             & (~ (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__empty_reg)))) {
            vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray 
                = vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_next;
        }
    } else {
        vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray = 0U;
    }
    vlSelfRef.async_fifo_tb__DOT__dut__DOT__empty_reg 
        = vlSelfRef.__Vdly__async_fifo_tb__DOT__dut__DOT__empty_reg;
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__bin 
        = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_bin)));
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__Vfuncout 
        = (0x0000001fU & ((IData)(__Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__bin) 
                          ^ VL_SHIFTR_III(5,5,32, (IData)(__Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__bin), 1U)));
    vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_next 
        = __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__Vfuncout;
}

void Vasync_fifo_tb___024root___eval_nba(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_nba\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x000000000000000cULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vasync_fifo_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo_tb___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo_tb___024root___nba_sequent__TOP__2(vlSelf);
    }
    if ((0x000000000000000cULL & vlSelfRef.__VnbaTriggered
         [0U])) {
        Vasync_fifo_tb___024root___nba_sequent__TOP__3(vlSelf);
    }
}

void Vasync_fifo_tb___024root___timing_commit(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___timing_commit\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((! (1ULL & vlSelfRef.__VactTriggered[0U]))) {
        vlSelfRef.__VtrigSched_he0f06744__0.commit(
                                                   "@(posedge async_fifo_tb.wr_clk)");
    }
    if ((! (4ULL & vlSelfRef.__VactTriggered[0U]))) {
        vlSelfRef.__VtrigSched_hd1c90b96__0.commit(
                                                   "@(posedge async_fifo_tb.rd_clk)");
    }
}

void Vasync_fifo_tb___024root___timing_resume(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___timing_resume\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_he0f06744__0.resume(
                                                   "@(posedge async_fifo_tb.wr_clk)");
    }
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_hd1c90b96__0.resume(
                                                   "@(posedge async_fifo_tb.rd_clk)");
    }
    if ((0x0000000000000010ULL & vlSelfRef.__VactTriggered
         [0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vasync_fifo_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vasync_fifo_tb___024root___eval_phase__act(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_phase__act\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vasync_fifo_tb___024root___eval_triggers__act(vlSelf);
    Vasync_fifo_tb___024root___timing_commit(vlSelf);
    Vasync_fifo_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vasync_fifo_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vasync_fifo_tb___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

void Vasync_fifo_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vasync_fifo_tb___024root___eval_phase__nba(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_phase__nba\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vasync_fifo_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vasync_fifo_tb___024root___eval_nba(vlSelf);
        Vasync_fifo_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vasync_fifo_tb___024root___eval(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vasync_fifo_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/async_fifo_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vasync_fifo_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/async_fifo_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vasync_fifo_tb___024root___eval_phase__act(vlSelf));
    } while (Vasync_fifo_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vasync_fifo_tb___024root___eval_debug_assertions(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_debug_assertions\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
