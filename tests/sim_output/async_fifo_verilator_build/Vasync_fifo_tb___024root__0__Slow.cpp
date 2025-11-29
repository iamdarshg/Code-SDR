// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vasync_fifo_tb.h for the primary calling header

#include "Vasync_fifo_tb__pch.h"

VL_ATTR_COLD void Vasync_fifo_tb___024root___eval_static__TOP(Vasync_fifo_tb___024root* vlSelf);

VL_ATTR_COLD void Vasync_fifo_tb___024root___eval_static(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_static\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vasync_fifo_tb___024root___eval_static__TOP(vlSelf);
    vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__wr_clk__0 
        = vlSelfRef.async_fifo_tb__DOT__wr_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__wr_rst_n__0 
        = vlSelfRef.async_fifo_tb__DOT__wr_rst_n;
    vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__rd_clk__0 
        = vlSelfRef.async_fifo_tb__DOT__rd_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__async_fifo_tb__DOT__rd_rst_n__0 
        = vlSelfRef.async_fifo_tb__DOT__rd_rst_n;
}

VL_ATTR_COLD void Vasync_fifo_tb___024root___eval_static__TOP(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_static__TOP\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.async_fifo_tb__DOT__test_completed = 0U;
}

VL_ATTR_COLD void Vasync_fifo_tb___024root___eval_final(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_final\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vasync_fifo_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vasync_fifo_tb___024root___eval_phase__stl(Vasync_fifo_tb___024root* vlSelf);

VL_ATTR_COLD void Vasync_fifo_tb___024root___eval_settle(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_settle\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vasync_fifo_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("verilog/async_fifo_tb.v", 3, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vasync_fifo_tb___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vasync_fifo_tb___024root___eval_triggers__stl(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_triggers__stl\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vasync_fifo_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vasync_fifo_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vasync_fifo_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vasync_fifo_tb___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vasync_fifo_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vasync_fifo_tb___024root___stl_sequent__TOP__0(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___stl_sequent__TOP__0\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*4:0*/ __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__Vfuncout;
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__Vfuncout = 0;
    CData/*4:0*/ __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__bin;
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__bin = 0;
    CData/*4:0*/ __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__Vfuncout;
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__Vfuncout = 0;
    CData/*4:0*/ __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__bin;
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__bin = 0;
    // Body
    vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_bin_next 
        = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_bin)));
    vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_bin_next 
        = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_bin)));
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__bin 
        = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_bin)));
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__Vfuncout 
        = (0x0000001fU & ((IData)(__Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__bin) 
                          ^ VL_SHIFTR_III(5,5,32, (IData)(__Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__bin), 1U)));
    vlSelfRef.async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_next 
        = __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__0__Vfuncout;
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__bin 
        = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_bin)));
    __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__Vfuncout 
        = (0x0000001fU & ((IData)(__Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__bin) 
                          ^ VL_SHIFTR_III(5,5,32, (IData)(__Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__bin), 1U)));
    vlSelfRef.async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_next 
        = __Vfunc_async_fifo_tb__DOT__dut__DOT__bin_to_gray__1__Vfuncout;
}

VL_ATTR_COLD void Vasync_fifo_tb___024root___eval_stl(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_stl\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vasync_fifo_tb___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vasync_fifo_tb___024root___eval_phase__stl(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___eval_phase__stl\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vasync_fifo_tb___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vasync_fifo_tb___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vasync_fifo_tb___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vasync_fifo_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vasync_fifo_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vasync_fifo_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge async_fifo_tb.wr_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge async_fifo_tb.wr_rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @(posedge async_fifo_tb.rd_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @(negedge async_fifo_tb.rd_rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vasync_fifo_tb___024root___ctor_var_reset(Vasync_fifo_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo_tb___024root___ctor_var_reset\n"); );
    Vasync_fifo_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->async_fifo_tb__DOT__wr_clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10117451583623634367ull);
    vlSelf->async_fifo_tb__DOT__rd_clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3646228978771442223ull);
    vlSelf->async_fifo_tb__DOT__wr_rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 474037418149267912ull);
    vlSelf->async_fifo_tb__DOT__rd_rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14619985844388814091ull);
    vlSelf->async_fifo_tb__DOT__din = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12739054191648887439ull);
    vlSelf->async_fifo_tb__DOT__wr_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12856826796052574469ull);
    vlSelf->async_fifo_tb__DOT__rd_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7123660458503422375ull);
    vlSelf->async_fifo_tb__DOT__i = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 603742696955201622ull);
    vlSelf->async_fifo_tb__DOT__expected_data = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 18037154127660507720ull);
    vlSelf->async_fifo_tb__DOT__test_passed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13171137604529400610ull);
    vlSelf->async_fifo_tb__DOT__test_completed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13404838347945757424ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__wr_ptr_gray = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 1341038187514809025ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__wr_ptr_bin = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 10863322332176549953ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_next = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 9397836169174434711ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__wr_ptr_bin_next = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 16292632809008340699ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__rd_ptr_gray = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 7600182551552087346ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__rd_ptr_bin = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 10656015493244550851ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_next = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 7379020215065488443ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__rd_ptr_bin_next = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 16969152900936746858ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_sync1 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 14153801527184709872ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__wr_ptr_gray_sync2 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 6733134182748704941ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_sync1 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 7307380417081234755ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__rd_ptr_gray_sync2 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3517992101889876649ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__full_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7654165807663629027ull);
    vlSelf->async_fifo_tb__DOT__dut__DOT__empty_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6736183427641195740ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->async_fifo_tb__DOT__dut__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 15194551390441333353ull);
    }
    vlSelf->async_fifo_tb__DOT__dut__DOT__dout_reg = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5770144173542824172ull);
    vlSelf->__Vdly__async_fifo_tb__DOT__dut__DOT__empty_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 849847552347201193ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__async_fifo_tb__DOT__wr_clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17336326702632956009ull);
    vlSelf->__Vtrigprevexpr___TOP__async_fifo_tb__DOT__wr_rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16832032570506997213ull);
    vlSelf->__Vtrigprevexpr___TOP__async_fifo_tb__DOT__rd_clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17405199139383609728ull);
    vlSelf->__Vtrigprevexpr___TOP__async_fifo_tb__DOT__rd_rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11193194765364562671ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
