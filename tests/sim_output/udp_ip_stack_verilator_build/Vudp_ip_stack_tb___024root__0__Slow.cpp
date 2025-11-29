// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vudp_ip_stack_tb.h for the primary calling header

#include "Vudp_ip_stack_tb__pch.h"

VL_ATTR_COLD void Vudp_ip_stack_tb___024root___eval_static(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_static\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__udp_ip_stack_tb__DOT__clk__0 
        = vlSelfRef.udp_ip_stack_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__udp_ip_stack_tb__DOT__rst_n__0 
        = vlSelfRef.udp_ip_stack_tb__DOT__rst_n;
}

VL_ATTR_COLD void Vudp_ip_stack_tb___024root___eval_final(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_final\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vudp_ip_stack_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vudp_ip_stack_tb___024root___eval_phase__stl(Vudp_ip_stack_tb___024root* vlSelf);

VL_ATTR_COLD void Vudp_ip_stack_tb___024root___eval_settle(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_settle\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vudp_ip_stack_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("verilog/udp_ip_stack_tb.v", 3, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vudp_ip_stack_tb___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vudp_ip_stack_tb___024root___eval_triggers__stl(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_triggers__stl\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vudp_ip_stack_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vudp_ip_stack_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vudp_ip_stack_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vudp_ip_stack_tb___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vudp_ip_stack_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vudp_ip_stack_tb___024root___stl_sequent__TOP__0(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___stl_sequent__TOP__0\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.udp_ip_stack_tb__DOT__app_ready = (0U 
                                                 == (IData)(vlSelfRef.udp_ip_stack_tb__DOT__dut__DOT__frame_state));
}

VL_ATTR_COLD void Vudp_ip_stack_tb___024root___eval_stl(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_stl\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vudp_ip_stack_tb___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vudp_ip_stack_tb___024root___eval_phase__stl(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___eval_phase__stl\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vudp_ip_stack_tb___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vudp_ip_stack_tb___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vudp_ip_stack_tb___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vudp_ip_stack_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vudp_ip_stack_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vudp_ip_stack_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge udp_ip_stack_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge udp_ip_stack_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vudp_ip_stack_tb___024root___ctor_var_reset(Vudp_ip_stack_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vudp_ip_stack_tb___024root___ctor_var_reset\n"); );
    Vudp_ip_stack_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->udp_ip_stack_tb__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12638987186339057368ull);
    vlSelf->udp_ip_stack_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 427465566076763664ull);
    vlSelf->udp_ip_stack_tb__DOT__app_len = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8158509457624499424ull);
    vlSelf->udp_ip_stack_tb__DOT__app_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5382397639898853839ull);
    vlSelf->udp_ip_stack_tb__DOT__app_ready = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4436491063480968902ull);
    vlSelf->udp_ip_stack_tb__DOT__dut__DOT__udp_length_reg = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3193278525986025443ull);
    vlSelf->udp_ip_stack_tb__DOT__dut__DOT__frame_state = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 13514408036833509517ull);
    vlSelf->udp_ip_stack_tb__DOT__dut__DOT__byte_counter = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3903702215199276189ull);
    vlSelf->udp_ip_stack_tb__DOT__dut__DOT__total_bytes = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11388630144683427375ull);
    vlSelf->udp_ip_stack_tb__DOT__dut__DOT__mac_len_reg = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6434531086779072188ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__udp_ip_stack_tb__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18078271132209873491ull);
    vlSelf->__Vtrigprevexpr___TOP__udp_ip_stack_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3534110118514411041ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
