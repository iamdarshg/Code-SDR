// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vhamming_window_tb.h for the primary calling header

#include "Vhamming_window_tb__pch.h"

VL_ATTR_COLD void Vhamming_window_tb___024root___eval_static__TOP(Vhamming_window_tb___024root* vlSelf);

VL_ATTR_COLD void Vhamming_window_tb___024root___eval_static(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_static\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vhamming_window_tb___024root___eval_static__TOP(vlSelf);
    vlSelfRef.__Vtrigprevexpr___TOP__hamming_window_tb__DOT__clk__0 
        = vlSelfRef.hamming_window_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__hamming_window_tb__DOT__rst_n__0 
        = vlSelfRef.hamming_window_tb__DOT__rst_n;
}

VL_ATTR_COLD void Vhamming_window_tb___024root___eval_static__TOP(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_static__TOP\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.hamming_window_tb__DOT__output_count = 0U;
}

VL_ATTR_COLD void Vhamming_window_tb___024root___eval_initial__TOP(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_initial__TOP\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0U] = 0x0a3dU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[1U] = 0x0a88U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[2U] = 0x0b68U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[3U] = 0x0cdbU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[4U] = 0x0edcU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[5U] = 0x1168U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[6U] = 0x1478U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[7U] = 0x1803U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[8U] = 0x1c01U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[9U] = 0x2068U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x0aU] = 0x252cU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x0bU] = 0x2a41U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x0cU] = 0x2f9bU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x0dU] = 0x352bU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x0eU] = 0x3ae4U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x0fU] = 0x40b7U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x10U] = 0x4695U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x11U] = 0x4c70U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x12U] = 0x5238U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x13U] = 0x57deU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x14U] = 0x5d55U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x15U] = 0x628eU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x16U] = 0x677cU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x17U] = 0x6c13U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x18U] = 0x7047U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x19U] = 0x740dU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x1aU] = 0x775bU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x1bU] = 0x7a2aU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x1cU] = 0x7c71U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x1dU] = 0x7e2cU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x1eU] = 0x7f56U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x1fU] = 0x7fecU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x20U] = 0x7fecU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x21U] = 0x7f56U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x22U] = 0x7e2cU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x23U] = 0x7c71U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x24U] = 0x7a2aU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x25U] = 0x775bU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x26U] = 0x740dU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x27U] = 0x7047U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x28U] = 0x6c13U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x29U] = 0x677cU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x2aU] = 0x628eU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x2bU] = 0x5d55U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x2cU] = 0x57deU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x2dU] = 0x5238U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x2eU] = 0x4c70U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x2fU] = 0x4695U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x30U] = 0x40b7U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x31U] = 0x3ae4U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x32U] = 0x352bU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x33U] = 0x2f9bU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x34U] = 0x2a41U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x35U] = 0x252cU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x36U] = 0x2068U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x37U] = 0x1c01U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x38U] = 0x1803U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x39U] = 0x1478U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x3aU] = 0x1168U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x3bU] = 0x0edcU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x3cU] = 0x0cdbU;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x3dU] = 0x0b68U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x3eU] = 0x0a88U;
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff[0x3fU] = 0x0a3dU;
}

VL_ATTR_COLD void Vhamming_window_tb___024root___eval_final(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_final\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhamming_window_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vhamming_window_tb___024root___eval_phase__stl(Vhamming_window_tb___024root* vlSelf);

VL_ATTR_COLD void Vhamming_window_tb___024root___eval_settle(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_settle\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vhamming_window_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("verilog/hamming_window_tb.v", 3, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vhamming_window_tb___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vhamming_window_tb___024root___eval_triggers__stl(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_triggers__stl\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vhamming_window_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vhamming_window_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhamming_window_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vhamming_window_tb___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vhamming_window_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vhamming_window_tb___024root___stl_sequent__TOP__0(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___stl_sequent__TOP__0\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.hamming_window_tb__DOT__dut__DOT__current_coeff 
        = vlSelfRef.hamming_window_tb__DOT__dut__DOT__window_coeff
        [(0x0000003fU & (IData)(vlSelfRef.hamming_window_tb__DOT__dut__DOT__sample_counter))];
}

VL_ATTR_COLD void Vhamming_window_tb___024root___eval_stl(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_stl\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vhamming_window_tb___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vhamming_window_tb___024root___eval_phase__stl(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___eval_phase__stl\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vhamming_window_tb___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vhamming_window_tb___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vhamming_window_tb___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vhamming_window_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vhamming_window_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vhamming_window_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge hamming_window_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge hamming_window_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vhamming_window_tb___024root___ctor_var_reset(Vhamming_window_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vhamming_window_tb___024root___ctor_var_reset\n"); );
    Vhamming_window_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->hamming_window_tb__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11283850439154276941ull);
    vlSelf->hamming_window_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8456743052916766176ull);
    vlSelf->hamming_window_tb__DOT__data_in = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 6624444893018553402ull);
    vlSelf->hamming_window_tb__DOT__data_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9768110245806164450ull);
    vlSelf->hamming_window_tb__DOT__output_count = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5470726074807199918ull);
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->hamming_window_tb__DOT__dut__DOT__window_coeff[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6629708139501321152ull);
    }
    vlSelf->hamming_window_tb__DOT__dut__DOT__sample_counter = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 10635985952743543541ull);
    vlSelf->hamming_window_tb__DOT__dut__DOT__buffer_full = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10040741509674436169ull);
    vlSelf->hamming_window_tb__DOT__dut__DOT__current_coeff = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 2546816334298569521ull);
    vlSelf->hamming_window_tb__DOT__dut__DOT__windowed_data = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 1165610521217268922ull);
    vlSelf->hamming_window_tb__DOT__dut__DOT__output_valid_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3330801933093616731ull);
    vlSelf->hamming_window_tb__DOT__dut__DOT__output_delay = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 4071004118320638273ull);
    vlSelf->__Vdly__hamming_window_tb__DOT__dut__DOT__sample_counter = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 13392198384905109629ull);
    vlSelf->__Vdly__hamming_window_tb__DOT__dut__DOT__output_delay = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 14448562383283464137ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__hamming_window_tb__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9002524684462574437ull);
    vlSelf->__Vtrigprevexpr___TOP__hamming_window_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15462864048134173403ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
