// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vrp2040_interface_tb.h for the primary calling header

#include "Vrp2040_interface_tb__pch.h"

VlCoroutine Vrp2040_interface_tb___024root___eval_initial__TOP__Vtiming__0(Vrp2040_interface_tb___024root* vlSelf);
VlCoroutine Vrp2040_interface_tb___024root___eval_initial__TOP__Vtiming__1(Vrp2040_interface_tb___024root* vlSelf);

void Vrp2040_interface_tb___024root___eval_initial(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_initial\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vrp2040_interface_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vrp2040_interface_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vrp2040_interface_tb___024root___eval_initial__TOP__Vtiming__0(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.rp2040_interface_tb__DOT__spi_clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x000000000000c350ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             66);
        vlSelfRef.rp2040_interface_tb__DOT__spi_clk 
            = (1U & (~ (IData)(vlSelfRef.rp2040_interface_tb__DOT__spi_clk)));
    }
}

VlCoroutine Vrp2040_interface_tb___024root___eval_initial__TOP__Vtiming__1(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i;
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 0;
    CData/*7:0*/ __Vtask_rp2040_interface_tb__DOT__spi_send_byte__0__addr;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__0__addr = 0;
    IData/*31:0*/ __Vtask_rp2040_interface_tb__DOT__spi_send_byte__0__data;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__0__data = 0;
    CData/*7:0*/ __Vtask_rp2040_interface_tb__DOT__spi_send_byte__1__addr;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__1__addr = 0;
    IData/*31:0*/ __Vtask_rp2040_interface_tb__DOT__spi_send_byte__1__data;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__1__data = 0;
    CData/*7:0*/ __Vtask_rp2040_interface_tb__DOT__spi_send_byte__2__addr;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__2__addr = 0;
    IData/*31:0*/ __Vtask_rp2040_interface_tb__DOT__spi_send_byte__2__data;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__2__data = 0;
    CData/*7:0*/ __Vtask_rp2040_interface_tb__DOT__spi_send_byte__3__addr;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__3__addr = 0;
    IData/*31:0*/ __Vtask_rp2040_interface_tb__DOT__spi_send_byte__3__data;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__3__data = 0;
    CData/*7:0*/ __Vtask_rp2040_interface_tb__DOT__spi_send_byte__4__addr;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__4__addr = 0;
    IData/*31:0*/ __Vtask_rp2040_interface_tb__DOT__spi_send_byte__4__data;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__4__data = 0;
    // Body
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 1U;
    vlSelfRef.rp2040_interface_tb__DOT__spi_mosi = 0U;
    vlSelfRef.rp2040_interface_tb__DOT__status_reg = 0xabcdU;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000f4240ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         105);
    VL_WRITEF_NX("Testing RP2040 Interface...\nWriting to frequency control register...\n",0);
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__0__data = 0x12345678U;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__0__addr = 0U;
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         75);
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 7U;
    while (VL_LTES_III(32, 0U, rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)) {
        vlSelfRef.rp2040_interface_tb__DOT__spi_mosi 
            = (1U & ((IData)(__Vtask_rp2040_interface_tb__DOT__spi_send_byte__0__addr) 
                     >> (7U & rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             80);
        rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
            = (rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
               - (IData)(1U));
    }
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 0x0000001fU;
    while (VL_LTES_III(32, 0U, rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)) {
        vlSelfRef.rp2040_interface_tb__DOT__spi_mosi 
            = (1U & (__Vtask_rp2040_interface_tb__DOT__spi_send_byte__0__data 
                     >> (0x0000001fU & rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             86);
        rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
            = (rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
               - (IData)(1U));
    }
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         91);
    co_await vlSelfRef.__VdlySched.delay(0x000000000007a120ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         113);
    VL_WRITEF_NX("Frequency word: 0x%x\nWriting to processing mode register...\n",0,
                 32,vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__freq_word_reg);
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__1__data = 2U;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__1__addr = 0x20U;
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         75);
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 7U;
    while (VL_LTES_III(32, 0U, rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)) {
        vlSelfRef.rp2040_interface_tb__DOT__spi_mosi 
            = (1U & ((IData)(__Vtask_rp2040_interface_tb__DOT__spi_send_byte__1__addr) 
                     >> (7U & rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             80);
        rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
            = (rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
               - (IData)(1U));
    }
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 0x0000001fU;
    while (VL_LTES_III(32, 0U, rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)) {
        vlSelfRef.rp2040_interface_tb__DOT__spi_mosi 
            = (1U & (__Vtask_rp2040_interface_tb__DOT__spi_send_byte__1__data 
                     >> (0x0000001fU & rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             86);
        rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
            = (rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
               - (IData)(1U));
    }
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         91);
    co_await vlSelfRef.__VdlySched.delay(0x000000000007a120ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         120);
    VL_WRITEF_NX("Processing mode: %1#\nTesting gain control...\n",0,
                 3,vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__proc_mode_reg);
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__2__data = 0x0000007fU;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__2__addr = 1U;
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         75);
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 7U;
    while (VL_LTES_III(32, 0U, rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)) {
        vlSelfRef.rp2040_interface_tb__DOT__spi_mosi 
            = (1U & ((IData)(__Vtask_rp2040_interface_tb__DOT__spi_send_byte__2__addr) 
                     >> (7U & rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             80);
        rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
            = (rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
               - (IData)(1U));
    }
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 0x0000001fU;
    while (VL_LTES_III(32, 0U, rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)) {
        vlSelfRef.rp2040_interface_tb__DOT__spi_mosi 
            = (1U & (__Vtask_rp2040_interface_tb__DOT__spi_send_byte__2__data 
                     >> (0x0000001fU & rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             86);
        rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
            = (rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
               - (IData)(1U));
    }
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         91);
    co_await vlSelfRef.__VdlySched.delay(0x000000000007a120ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         127);
    VL_WRITEF_NX("Gain control: 0x%x\nSetting modulation_type to AM...\n",0,
                 8,vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__gain_ctrl_reg);
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__3__data = 1U;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__3__addr = 0x21U;
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         75);
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 7U;
    while (VL_LTES_III(32, 0U, rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)) {
        vlSelfRef.rp2040_interface_tb__DOT__spi_mosi 
            = (1U & ((IData)(__Vtask_rp2040_interface_tb__DOT__spi_send_byte__3__addr) 
                     >> (7U & rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             80);
        rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
            = (rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
               - (IData)(1U));
    }
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 0x0000001fU;
    while (VL_LTES_III(32, 0U, rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)) {
        vlSelfRef.rp2040_interface_tb__DOT__spi_mosi 
            = (1U & (__Vtask_rp2040_interface_tb__DOT__spi_send_byte__3__data 
                     >> (0x0000001fU & rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             86);
        rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
            = (rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
               - (IData)(1U));
    }
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         91);
    co_await vlSelfRef.__VdlySched.delay(0x000000000007a120ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         135);
    VL_WRITEF_NX("Modulation type: 0x%x\nTesting clock gating enable...\n",0,
                 8,vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__demod_type_reg);
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__4__data = 1U;
    __Vtask_rp2040_interface_tb__DOT__spi_send_byte__4__addr = 0x23U;
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         75);
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 7U;
    while (VL_LTES_III(32, 0U, rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)) {
        vlSelfRef.rp2040_interface_tb__DOT__spi_mosi 
            = (1U & ((IData)(__Vtask_rp2040_interface_tb__DOT__spi_send_byte__4__addr) 
                     >> (7U & rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             80);
        rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
            = (rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
               - (IData)(1U));
    }
    rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i = 0x0000001fU;
    while (VL_LTES_III(32, 0U, rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)) {
        vlSelfRef.rp2040_interface_tb__DOT__spi_mosi 
            = (1U & (__Vtask_rp2040_interface_tb__DOT__spi_send_byte__4__data 
                     >> (0x0000001fU & rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                             nullptr, 
                                             "verilog/rp2040_interface_tb.v", 
                                             86);
        rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
            = (rp2040_interface_tb__DOT__spi_send_byte__Vstatic__i 
               - (IData)(1U));
    }
    vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         91);
    co_await vlSelfRef.__VdlySched.delay(0x000000000007a120ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         143);
    VL_WRITEF_NX("Clock gating: %b\n",0,1,vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__clk_gating_reg);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000f4240ULL, 
                                         nullptr, "verilog/rp2040_interface_tb.v", 
                                         147);
    VL_WRITEF_NX("RP2040 Interface test completed\n",0);
    if ((0x12345678U == vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__freq_word_reg)) {
        VL_WRITEF_NX("\342\234\223 Frequency word write successful\n",0);
    } else {
        VL_WRITEF_NX("\342\234\227 Frequency word write failed\n",0);
    }
    if ((2U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__proc_mode_reg))) {
        VL_WRITEF_NX("\342\234\223 Processing mode write successful\n",0);
    } else {
        VL_WRITEF_NX("\342\234\227 Processing mode write failed\n",0);
    }
    VL_FINISH_MT("verilog/rp2040_interface_tb.v", 164, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vrp2040_interface_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vrp2040_interface_tb___024root___eval_triggers__act(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_triggers__act\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 2U) 
                                                     | ((((~ (IData)(vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__rp2040_interface_tb__DOT__spi_cs_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.rp2040_interface_tb__DOT__spi_clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__rp2040_interface_tb__DOT__spi_clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__rp2040_interface_tb__DOT__spi_clk__0 
        = vlSelfRef.rp2040_interface_tb__DOT__spi_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rp2040_interface_tb__DOT__spi_cs_n__0 
        = vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vrp2040_interface_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vrp2040_interface_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___trigger_anySet__act\n"); );
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

void Vrp2040_interface_tb___024root___nba_sequent__TOP__0(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___nba_sequent__TOP__0\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_addr;
    __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_addr = 0;
    CData/*1:0*/ __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_state;
    __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_state = 0;
    CData/*3:0*/ __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter;
    __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter = 0;
    IData/*31:0*/ __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_out;
    __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_out = 0;
    IData/*31:0*/ __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_in;
    __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_in = 0;
    // Body
    __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_addr 
        = vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr;
    __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_state 
        = vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_state;
    __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter 
        = vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter;
    __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_out 
        = vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_out;
    __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_in 
        = vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_in;
    if (vlSelfRef.rp2040_interface_tb__DOT__spi_cs_n) {
        if ((0U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_state))) {
            __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_addr 
                = vlSelfRef.rp2040_interface_tb__DOT__spi_mosi;
            __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_state = 1U;
            __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter = 0U;
        } else if ((1U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_state))) {
            __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter 
                = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter)));
            if ((7U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter))) {
                __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_state = 2U;
                __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter = 0U;
                __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_out 
                    = ((0x10U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))
                        ? (IData)(vlSelfRef.rp2040_interface_tb__DOT__status_reg)
                        : 0U);
            }
        } else if ((2U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_state))) {
            __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter 
                = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter)));
            __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_out 
                = (vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_out 
                   << 1U);
            if ((8U > (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter))) {
                __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_in 
                    = ((vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_in 
                        << 1U) | (IData)(vlSelfRef.rp2040_interface_tb__DOT__spi_mosi));
            } else if ((8U > (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter))) {
                __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_in 
                    = ((vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_in 
                        << 1U) | (IData)(vlSelfRef.rp2040_interface_tb__DOT__spi_mosi));
            } else {
                __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_in 
                    = ((vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_in 
                        << 1U) | (IData)(vlSelfRef.rp2040_interface_tb__DOT__spi_mosi));
                __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_state = 0U;
            }
        } else {
            __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_state = 0U;
        }
        if (((2U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_state)) 
             & (0x0fU == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter)))) {
            if (((((((((0U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr)) 
                       | (1U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                      | (2U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                     | (3U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                    | (4U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                   | (5U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                  | (0x20U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                 | (0x21U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr)))) {
                if ((0U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                    vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__freq_word_reg 
                        = vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_in;
                }
                if ((0U != (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                    if ((1U != (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                        if ((2U != (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                            if ((3U != (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                                if ((4U != (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                                    if ((5U != (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                                        if ((0x20U 
                                             != (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                                            vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__demod_type_reg 
                                                = (0x000000ffU 
                                                   & vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_in);
                                        }
                                        if ((0x20U 
                                             == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                                            vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__proc_mode_reg 
                                                = (7U 
                                                   & vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_in);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if ((1U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                        vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__gain_ctrl_reg 
                            = (0x000000ffU & vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_in);
                    }
                }
            }
            if ((1U & (~ ((((((((0U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr)) 
                                | (1U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                               | (2U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                              | (3U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                             | (4U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                            | (5U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                           | (0x20U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) 
                          | (0x21U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr)))))) {
                if ((0x22U != (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                    if ((0x23U == (IData)(vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr))) {
                        vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__clk_gating_reg 
                            = (1U & vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_in);
                    }
                }
            }
        }
    } else {
        __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_state = 0U;
        __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_addr = 0U;
        __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_in = 0U;
        __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter = 0U;
        vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__freq_word_reg = 0x40000000U;
        vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__demod_type_reg = 0U;
        vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__gain_ctrl_reg = 0x80U;
        vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__proc_mode_reg = 0U;
        vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__clk_gating_reg = 0U;
    }
    vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_out 
        = __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_out;
    vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_state 
        = __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_state;
    vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter 
        = __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_bit_counter;
    vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_addr 
        = __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_addr;
    vlSelfRef.rp2040_interface_tb__DOT__dut__DOT__spi_data_in 
        = __Vdly__rp2040_interface_tb__DOT__dut__DOT__spi_data_in;
}

void Vrp2040_interface_tb___024root___eval_nba(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_nba\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vrp2040_interface_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
}

void Vrp2040_interface_tb___024root___timing_resume(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___timing_resume\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vrp2040_interface_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vrp2040_interface_tb___024root___eval_phase__act(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_phase__act\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vrp2040_interface_tb___024root___eval_triggers__act(vlSelf);
    Vrp2040_interface_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vrp2040_interface_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vrp2040_interface_tb___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

void Vrp2040_interface_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vrp2040_interface_tb___024root___eval_phase__nba(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_phase__nba\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vrp2040_interface_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vrp2040_interface_tb___024root___eval_nba(vlSelf);
        Vrp2040_interface_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vrp2040_interface_tb___024root___eval(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vrp2040_interface_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/rp2040_interface_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vrp2040_interface_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/rp2040_interface_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vrp2040_interface_tb___024root___eval_phase__act(vlSelf));
    } while (Vrp2040_interface_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vrp2040_interface_tb___024root___eval_debug_assertions(Vrp2040_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrp2040_interface_tb___024root___eval_debug_assertions\n"); );
    Vrp2040_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
