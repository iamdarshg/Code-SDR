// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vadc_interface_tb.h for the primary calling header

#include "Vadc_interface_tb__pch.h"

VlCoroutine Vadc_interface_tb___024root___eval_initial__TOP__Vtiming__0(Vadc_interface_tb___024root* vlSelf);
VlCoroutine Vadc_interface_tb___024root___eval_initial__TOP__Vtiming__1(Vadc_interface_tb___024root* vlSelf);

void Vadc_interface_tb___024root___eval_initial(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_initial\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vadc_interface_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vadc_interface_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vadc_interface_tb___024root___eval_initial__TOP__Vtiming__0(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.adc_interface_tb__DOT__clk_adc = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000002530ULL, 
                                             nullptr, 
                                             "verilog/adc_interface_tb.v", 
                                             35);
        vlSelfRef.adc_interface_tb__DOT__clk_adc = 
            (1U & (~ (IData)(vlSelfRef.adc_interface_tb__DOT__clk_adc)));
    }
}

VlCoroutine Vadc_interface_tb___024root___eval_initial__TOP__Vtiming__1(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.adc_interface_tb__DOT__rst_n = 0U;
    vlSelfRef.adc_interface_tb__DOT__adc_data = 0U;
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 0U;
    vlSelfRef.adc_interface_tb__DOT__adc_ovr = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/adc_interface_tb.v", 
                                         47);
    vlSelfRef.adc_interface_tb__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/adc_interface_tb.v", 
                                         51);
    VL_WRITEF_NX("Testing ADC interface...\n",0);
    co_await vlSelfRef.__VtrigSched_h6de165b8__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge adc_interface_tb.clk_adc)", 
                                                         "verilog/adc_interface_tb.v", 
                                                         57);
    vlSelfRef.adc_interface_tb__DOT__adc_data = 0x01ffU;
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002530ULL, 
                                         nullptr, "verilog/adc_interface_tb.v", 
                                         57);
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 0U;
    co_await vlSelfRef.__VtrigSched_h6de165b8__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge adc_interface_tb.clk_adc)", 
                                                         "verilog/adc_interface_tb.v", 
                                                         58);
    vlSelfRef.adc_interface_tb__DOT__adc_data = 0U;
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002530ULL, 
                                         nullptr, "verilog/adc_interface_tb.v", 
                                         58);
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 0U;
    co_await vlSelfRef.__VtrigSched_h6de165b8__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge adc_interface_tb.clk_adc)", 
                                                         "verilog/adc_interface_tb.v", 
                                                         59);
    vlSelfRef.adc_interface_tb__DOT__adc_data = 0x0200U;
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002530ULL, 
                                         nullptr, "verilog/adc_interface_tb.v", 
                                         59);
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 0U;
    co_await vlSelfRef.__VtrigSched_h6de165b8__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge adc_interface_tb.clk_adc)", 
                                                         "verilog/adc_interface_tb.v", 
                                                         60);
    vlSelfRef.adc_interface_tb__DOT__adc_data = 0x0100U;
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002530ULL, 
                                         nullptr, "verilog/adc_interface_tb.v", 
                                         60);
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 0U;
    co_await vlSelfRef.__VtrigSched_h6de165b8__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge adc_interface_tb.clk_adc)", 
                                                         "verilog/adc_interface_tb.v", 
                                                         63);
    vlSelfRef.adc_interface_tb__DOT__adc_data = 0U;
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 1U;
    vlSelfRef.adc_interface_tb__DOT__adc_ovr = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002530ULL, 
                                         nullptr, "verilog/adc_interface_tb.v", 
                                         63);
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 0U;
    vlSelfRef.adc_interface_tb__DOT__adc_ovr = 0U;
    co_await vlSelfRef.__VtrigSched_h6de165b8__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge adc_interface_tb.clk_adc)", 
                                                         "verilog/adc_interface_tb.v", 
                                                         64);
    vlSelfRef.adc_interface_tb__DOT__adc_data = 0U;
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 1U;
    vlSelfRef.adc_interface_tb__DOT__adc_ovr = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002530ULL, 
                                         nullptr, "verilog/adc_interface_tb.v", 
                                         64);
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 0U;
    vlSelfRef.adc_interface_tb__DOT__adc_ovr = 0U;
    co_await vlSelfRef.__VtrigSched_h6de165b8__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge adc_interface_tb.clk_adc)", 
                                                         "verilog/adc_interface_tb.v", 
                                                         65);
    vlSelfRef.adc_interface_tb__DOT__adc_data = 0U;
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 1U;
    vlSelfRef.adc_interface_tb__DOT__adc_ovr = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002530ULL, 
                                         nullptr, "verilog/adc_interface_tb.v", 
                                         65);
    vlSelfRef.adc_interface_tb__DOT__adc_valid = 0U;
    vlSelfRef.adc_interface_tb__DOT__adc_ovr = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x000000000007a120ULL, 
                                         nullptr, "verilog/adc_interface_tb.v", 
                                         68);
    VL_WRITEF_NX("ADC interface test completed\n",0);
    if (vlSelfRef.adc_interface_tb__DOT__dut__DOT__overflow_detect_reg) {
        VL_WRITEF_NX("\342\234\223 Overflow detection working\nTEST PASSED\n",0);
    } else {
        VL_WRITEF_NX("\342\234\227 Overflow detection failed\nTEST FAILED\n",0);
    }
    VL_FINISH_MT("verilog/adc_interface_tb.v", 81, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vadc_interface_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vadc_interface_tb___024root___eval_triggers__act(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_triggers__act\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 2U) 
                                                     | ((((~ (IData)(vlSelfRef.adc_interface_tb__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__adc_interface_tb__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.adc_interface_tb__DOT__clk_adc) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__adc_interface_tb__DOT__clk_adc__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__adc_interface_tb__DOT__clk_adc__0 
        = vlSelfRef.adc_interface_tb__DOT__clk_adc;
    vlSelfRef.__Vtrigprevexpr___TOP__adc_interface_tb__DOT__rst_n__0 
        = vlSelfRef.adc_interface_tb__DOT__rst_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vadc_interface_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vadc_interface_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___trigger_anySet__act\n"); );
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

extern const VlUnpacked<CData/*3:0*/, 128> Vadc_interface_tb__ConstPool__TABLE_hf4d6ab3c_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vadc_interface_tb__ConstPool__TABLE_hbdb1229e_0;

void Vadc_interface_tb___024root___nba_sequent__TOP__0(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___nba_sequent__TOP__0\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*6:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    // Body
    vlSelfRef.__Vdly__adc_interface_tb__DOT__dut__DOT__dc_offset 
        = vlSelfRef.adc_interface_tb__DOT__dut__DOT__dc_offset;
    __Vtableidx1 = (((IData)(vlSelfRef.adc_interface_tb__DOT__dut__DOT__overflow_detect_reg) 
                     << 6U) | (((IData)(vlSelfRef.adc_interface_tb__DOT__dut__DOT__overflow_count) 
                                << 2U) | (((IData)(vlSelfRef.adc_interface_tb__DOT__dut__DOT__adc_ovr_reg) 
                                           << 1U) | (IData)(vlSelfRef.adc_interface_tb__DOT__rst_n))));
    vlSelfRef.adc_interface_tb__DOT__dut__DOT__overflow_count 
        = Vadc_interface_tb__ConstPool__TABLE_hf4d6ab3c_0
        [__Vtableidx1];
    vlSelfRef.adc_interface_tb__DOT__dut__DOT__overflow_detect_reg 
        = Vadc_interface_tb__ConstPool__TABLE_hbdb1229e_0
        [__Vtableidx1];
    if (vlSelfRef.adc_interface_tb__DOT__rst_n) {
        if (vlSelfRef.adc_interface_tb__DOT__dut__DOT__adc_valid_reg) {
            if ((0xffffU > (IData)(vlSelfRef.adc_interface_tb__DOT__dut__DOT__avg_count))) {
                vlSelfRef.adc_interface_tb__DOT__dut__DOT__avg_count 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.adc_interface_tb__DOT__dut__DOT__avg_count)));
                vlSelfRef.__Vdly__adc_interface_tb__DOT__dut__DOT__dc_offset 
                    = (vlSelfRef.adc_interface_tb__DOT__dut__DOT__dc_offset 
                       + (((- (IData)((1U & ((IData)(vlSelfRef.adc_interface_tb__DOT__dut__DOT__adc_data_reg) 
                                             >> 9U)))) 
                           << 0x0000000aU) | (IData)(vlSelfRef.adc_interface_tb__DOT__dut__DOT__adc_data_reg)));
            }
        }
    } else {
        vlSelfRef.__Vdly__adc_interface_tb__DOT__dut__DOT__dc_offset = 0U;
        vlSelfRef.adc_interface_tb__DOT__dut__DOT__avg_count = 0U;
    }
    vlSelfRef.adc_interface_tb__DOT__dut__DOT__adc_ovr_reg 
        = ((IData)(vlSelfRef.adc_interface_tb__DOT__rst_n) 
           && (IData)(vlSelfRef.adc_interface_tb__DOT__adc_ovr));
}

void Vadc_interface_tb___024root___nba_sequent__TOP__1(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___nba_sequent__TOP__1\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY((vlSelfRef.adc_interface_tb__DOT__dut__DOT__sample_valid_sync))) {
        VL_WRITEF_NX("Time=%0t: ADC data=%x, Output data=%x\n",0,
                     64,VL_TIME_UNITED_Q(1000),-9,10,
                     (IData)(vlSelfRef.adc_interface_tb__DOT__adc_data),
                     10,(0x000003ffU & ((IData)(vlSelfRef.adc_interface_tb__DOT__dut__DOT__adc_data_reg) 
                                        - vlSelfRef.adc_interface_tb__DOT__dut__DOT__dc_offset)));
    }
}

void Vadc_interface_tb___024root___nba_sequent__TOP__2(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___nba_sequent__TOP__2\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.adc_interface_tb__DOT__dut__DOT__dc_offset 
        = vlSelfRef.__Vdly__adc_interface_tb__DOT__dut__DOT__dc_offset;
    vlSelfRef.adc_interface_tb__DOT__dut__DOT__sample_valid_sync 
        = ((IData)(vlSelfRef.adc_interface_tb__DOT__rst_n) 
           && (IData)(vlSelfRef.adc_interface_tb__DOT__dut__DOT__adc_valid_reg));
    vlSelfRef.adc_interface_tb__DOT__dut__DOT__adc_data_reg 
        = ((IData)(vlSelfRef.adc_interface_tb__DOT__rst_n)
            ? (IData)(vlSelfRef.adc_interface_tb__DOT__adc_data)
            : 0U);
    vlSelfRef.adc_interface_tb__DOT__dut__DOT__adc_valid_reg 
        = ((IData)(vlSelfRef.adc_interface_tb__DOT__rst_n) 
           && (IData)(vlSelfRef.adc_interface_tb__DOT__adc_valid));
}

void Vadc_interface_tb___024root___eval_nba(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_nba\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vadc_interface_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vadc_interface_tb___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vadc_interface_tb___024root___nba_sequent__TOP__2(vlSelf);
    }
}

void Vadc_interface_tb___024root___timing_commit(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___timing_commit\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((! (1ULL & vlSelfRef.__VactTriggered[0U]))) {
        vlSelfRef.__VtrigSched_h6de165b8__0.commit(
                                                   "@(posedge adc_interface_tb.clk_adc)");
    }
}

void Vadc_interface_tb___024root___timing_resume(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___timing_resume\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h6de165b8__0.resume(
                                                   "@(posedge adc_interface_tb.clk_adc)");
    }
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vadc_interface_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vadc_interface_tb___024root___eval_phase__act(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_phase__act\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vadc_interface_tb___024root___eval_triggers__act(vlSelf);
    Vadc_interface_tb___024root___timing_commit(vlSelf);
    Vadc_interface_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vadc_interface_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vadc_interface_tb___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

void Vadc_interface_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vadc_interface_tb___024root___eval_phase__nba(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_phase__nba\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vadc_interface_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vadc_interface_tb___024root___eval_nba(vlSelf);
        Vadc_interface_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vadc_interface_tb___024root___eval(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vadc_interface_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/adc_interface_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vadc_interface_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/adc_interface_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vadc_interface_tb___024root___eval_phase__act(vlSelf));
    } while (Vadc_interface_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vadc_interface_tb___024root___eval_debug_assertions(Vadc_interface_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vadc_interface_tb___024root___eval_debug_assertions\n"); );
    Vadc_interface_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
