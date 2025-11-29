// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vudp_ip_stack_tb.h for the primary calling header

#include "Vudp_ip_stack_tb__pch.h"

VlCoroutine Vudp_ip_stack_tb___024root___eval_initial__TOP__Vtiming__0(Vudp_ip_stack_tb___024root* vlSelf);
VlCoroutine Vudp_ip_stack_tb___024root___eval_initial__TOP__Vtiming__1(Vudp_ip_stack_tb___024root* vlSelf);

void Vudp_ip_stack_tb___024root___eval_initial(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_initial\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vudp_ip_stack_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vudp_ip_stack_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vudp_ip_stack_tb___024root___eval_initial__TOP__Vtiming__0(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.udp_ip_stack_tb__DOT__clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000000fa0ULL, 
                                             nullptr, 
                                             "verilog/udp_ip_stack_tb.v", 
                                             50);
        vlSelfRef.udp_ip_stack_tb__DOT__clk = (1U & 
                                               (~ (IData)(vlSelfRef.udp_ip_stack_tb__DOT__clk)));
    }
}

VlCoroutine Vudp_ip_stack_tb___024root___eval_initial__TOP__Vtiming__1(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.udp_ip_stack_tb__DOT__rst_n = 0U;
    vlSelfRef.udp_ip_stack_tb__DOT__app_len = 0U;
    vlSelfRef.udp_ip_stack_tb__DOT__app_valid = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/udp_ip_stack_tb.v", 
                                         66);
    vlSelfRef.udp_ip_stack_tb__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/udp_ip_stack_tb.v", 
                                         70);
    VL_WRITEF_NX("Testing UDP/IP Stack...\n",0);
    vlSelfRef.udp_ip_stack_tb__DOT__app_len = 4U;
    vlSelfRef.udp_ip_stack_tb__DOT__app_valid = 1U;
    while ((0U != (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__frame_state))) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001f40ULL, 
                                             nullptr, 
                                             "verilog/udp_ip_stack_tb.v", 
                                             80);
    }
    vlSelfRef.udp_ip_stack_tb__DOT__app_valid = 0U;
    VL_WRITEF_NX("Packet data sent, waiting for MAC layer processing...\n",0);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000f4240ULL, 
                                         nullptr, "verilog/udp_ip_stack_tb.v", 
                                         86);
    VL_WRITEF_NX("UDP/IP Stack test completed\n",0);
    if ((0U < (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__mac_len_reg))) {
        VL_WRITEF_NX("\342\234\223 Packet assembly functional, MAC length: %0# bytes\nTEST PASSED\n",0,
                     16,vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__mac_len_reg);
    } else {
        VL_WRITEF_NX("\342\232\240 No MAC output generated\nTEST FAILED\n",0);
    }
    VL_FINISH_MT("verilog/udp_ip_stack_tb.v", 98, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vudp_ip_stack_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vudp_ip_stack_tb___024root___eval_triggers__act(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_triggers__act\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 2U) 
                                                     | ((((~ (IData)(vlSelfRef.udp_ip_stack_tb__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__udp_ip_stack_tb__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.udp_ip_stack_tb__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__udp_ip_stack_tb__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__udp_ip_stack_tb__DOT__clk__0 
        = vlSelfRef.udp_ip_stack_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__udp_ip_stack_tb__DOT__rst_n__0 
        = vlSelfRef.udp_ip_stack_tb__DOT__rst_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vudp_ip_stack_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vudp_ip_stack_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___trigger_anySet__act\n"); );
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

void Vudp_ip_stack_tb___024root___nba_sequent__TOP__0(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___nba_sequent__TOP__0\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*2:0*/ __Vdly__udp_ip_stack_tb__DOT__dut__DOT__frame_state;
    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__frame_state = 0;
    SData/*15:0*/ __Vdly__udp_ip_stack_tb__DOT__dut__DOT__total_bytes;
    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__total_bytes = 0;
    CData/*4:0*/ __Vdly__udp_ip_stack_tb__DOT__dut__DOT__byte_counter;
    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__byte_counter = 0;
    // Body
    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__total_bytes 
        = vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__total_bytes;
    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__byte_counter 
        = vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__byte_counter;
    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__frame_state 
        = vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__frame_state;
    if (vlSelfRef.udp_ip_stack_tb__DOT__rst_n) {
        if ((4U & (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__frame_state))) {
            __Vdly__udp_ip_stack_tb__DOT__dut__DOT__frame_state = 0U;
        } else if ((2U & (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__frame_state))) {
            if ((1U & (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__frame_state))) {
                if (((IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__total_bytes) 
                     < (0x0000ffffU & ((IData)(0x0014U) 
                                       + (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__udp_length_reg))))) {
                    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__total_bytes 
                        = (0x0000ffffU & ((IData)(4U) 
                                          + (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__total_bytes)));
                    if (((IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__total_bytes) 
                         >= ((IData)(0x00000010U) + (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__udp_length_reg)))) {
                        __Vdly__udp_ip_stack_tb__DOT__dut__DOT__frame_state = 4U;
                    }
                }
            } else if ((8U > (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__byte_counter))) {
                __Vdly__udp_ip_stack_tb__DOT__dut__DOT__byte_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__byte_counter)));
                __Vdly__udp_ip_stack_tb__DOT__dut__DOT__total_bytes 
                    = (0x0000ffffU & ((IData)(4U) + (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__total_bytes)));
                if ((7U == (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__byte_counter))) {
                    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__frame_state = 3U;
                    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__byte_counter = 0U;
                }
            }
        } else if ((1U & (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__frame_state))) {
            if ((0x14U > (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__byte_counter))) {
                __Vdly__udp_ip_stack_tb__DOT__dut__DOT__total_bytes 
                    = (0x0000ffffU & ((IData)(4U) + (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__total_bytes)));
                __Vdly__udp_ip_stack_tb__DOT__dut__DOT__byte_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__byte_counter)));
                if ((0x13U == (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__byte_counter))) {
                    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__frame_state = 2U;
                    __Vdly__udp_ip_stack_tb__DOT__dut__DOT__byte_counter = 0U;
                }
            }
        } else if (((IData)(vlSelfRef.udp_ip_stack_tb__DOT__app_valid) 
                    & (IData)(vlSelfRef.udp_ip_stack_tb__DOT__app_ready))) {
            __Vdly__udp_ip_stack_tb__DOT__dut__DOT__frame_state = 1U;
            __Vdly__udp_ip_stack_tb__DOT__dut__DOT__byte_counter = 0U;
            __Vdly__udp_ip_stack_tb__DOT__dut__DOT__total_bytes = 0U;
        }
        if ((4U == (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__frame_state))) {
            vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__mac_len_reg 
                = vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__total_bytes;
        }
        if (vlSelfRef.udp_ip_stack_tb__DOT__app_valid) {
            vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__udp_length_reg 
                = (0x0000ffffU & ((IData)(8U) + (IData)(vlSelfRef.udp_ip_stack_tb__DOT__app_len)));
        }
    } else {
        __Vdly__udp_ip_stack_tb__DOT__dut__DOT__frame_state = 0U;
        __Vdly__udp_ip_stack_tb__DOT__dut__DOT__byte_counter = 0U;
        __Vdly__udp_ip_stack_tb__DOT__dut__DOT__total_bytes = 0U;
        vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__mac_len_reg = 0U;
        vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__udp_length_reg = 8U;
    }
    vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__byte_counter 
        = __Vdly__udp_ip_stack_tb__DOT__dut__DOT__byte_counter;
    vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__total_bytes 
        = __Vdly__udp_ip_stack_tb__DOT__dut__DOT__total_bytes;
    vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__frame_state 
        = __Vdly__udp_ip_stack_tb__DOT__dut__DOT__frame_state;
    vlSelfRef.udp_ip_stack_tb__DOT__app_ready = (0U 
                                                 == (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__frame_state));
}

void Vudp_ip_stack_tb___024root___eval_nba(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_nba\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vudp_ip_stack_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
}

void Vudp_ip_stack_tb___024root___timing_resume(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___timing_resume\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vudp_ip_stack_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vudp_ip_stack_tb___024root___eval_phase__act(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_phase__act\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vudp_ip_stack_tb___024root___eval_triggers__act(vlSelf);
    Vudp_ip_stack_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vudp_ip_stack_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vudp_ip_stack_tb___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

void Vudp_ip_stack_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vudp_ip_stack_tb___024root___eval_phase__nba(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_phase__nba\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vudp_ip_stack_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vudp_ip_stack_tb___024root___eval_nba(vlSelf);
        Vudp_ip_stack_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vudp_ip_stack_tb___024root___eval(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vudp_ip_stack_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/udp_ip_stack_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vudp_ip_stack_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/udp_ip_stack_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vudp_ip_stack_tb___024root___eval_phase__act(vlSelf));
    } while (Vudp_ip_stack_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vudp_ip_stack_tb___024root___eval_debug_assertions(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_debug_assertions\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
