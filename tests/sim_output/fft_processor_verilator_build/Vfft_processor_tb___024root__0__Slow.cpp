// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vfft_processor_tb.h for the primary calling header

#include "Vfft_processor_tb__pch.h"

VL_ATTR_COLD void Vfft_processor_tb___024root___eval_static(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_static\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__fft_processor_tb__DOT__clk__0 
        = vlSelfRef.fft_processor_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__fft_processor_tb__DOT__rst_n__0 
        = vlSelfRef.fft_processor_tb__DOT__rst_n;
}

VL_ATTR_COLD void Vfft_processor_tb___024root___eval_initial__TOP(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_initial__TOP\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__k;
    fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__k = 0;
    // Body
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg[0U] = 0U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg[0U] = 0U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg[0U] = 0U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg[0U] = 0U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg[0U] = 0U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg[1U] = 2U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg[2U] = 4U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg[3U] = 8U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg[4U] = 0x10U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg[5U] = 0x20U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg[6U] = 0x40U;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg[7U] = 0x80U;
    fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__k = 0U;
    while (VL_GTS_III(32, 0x00000080U, fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__k)) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__angle 
            = ((-6.28318530717958623e+00 * VL_ISTOR_D_I(32, fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__k)) 
               / 256.0);
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__cos_val 
            = cos(vlSelfRef.fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__angle);
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__sin_val 
            = sin(vlSelfRef.fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__angle);
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_real[(0x0000007fU 
                                                                 & fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__k)] 
            = (0x0000ffffU & VL_RTOI_I_D((3.27670000000000000e+04 
                                          * vlSelfRef.fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__cos_val)));
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_imag[(0x0000007fU 
                                                                 & fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__k)] 
            = (0x0000ffffU & VL_RTOI_I_D((3.27670000000000000e+04 
                                          * vlSelfRef.fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__sin_val)));
        fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__k 
            = ((IData)(1U) + fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__k);
    }
}

VL_ATTR_COLD void Vfft_processor_tb___024root___eval_final(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_final\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vfft_processor_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vfft_processor_tb___024root___eval_phase__stl(Vfft_processor_tb___024root* vlSelf);

VL_ATTR_COLD void Vfft_processor_tb___024root___eval_settle(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_settle\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vfft_processor_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("verilog/fft_processor_tb.v", 3, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vfft_processor_tb___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vfft_processor_tb___024root___eval_triggers__stl(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_triggers__stl\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vfft_processor_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vfft_processor_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vfft_processor_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vfft_processor_tb___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vfft_processor_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vfft_processor_tb___024root___stl_sequent__TOP__0(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___stl_sequent__TOP__0\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0;
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 = 0;
    SData/*11:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr = 0;
    // Body
    __Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr 
        = vlSelfRef.fft_processor_tb__DOT__dut__DOT__sample_counter;
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 0x0bU));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0ffeU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | (IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 0x0aU));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0ffdU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 1U));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 9U));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0ffbU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 2U));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 8U));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0ff7U & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 3U));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 7U));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0fefU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 4U));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 6U));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0fdfU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 5U));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 5U));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0fbfU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 6U));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 4U));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0f7fU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 7U));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 3U));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0effU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 8U));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 2U));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0dffU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 9U));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & ((IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr) 
                 >> 1U));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x0bffU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 0x0000000aU));
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 
        = (1U & (IData)(__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr));
    vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout 
        = ((0x07ffU & (IData)(vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout)) 
           | ((IData)(fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0) 
              << 0x0000000bU));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__bit_reversed_addr 
        = vlSelfRef.__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg[1U] 
        = (1U & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
           [1U]);
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg[2U] 
        = (3U & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
           [2U]);
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg[3U] 
        = (7U & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
           [3U]);
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg[4U] 
        = (0x000fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
           [4U]);
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg[5U] 
        = (0x001fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
           [5U]);
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg[6U] 
        = (0x003fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
           [6U]);
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg[7U] 
        = (0x007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
           [7U]);
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg[1U] 
        = (0x0ffcU & VL_SHIFTL_III(12,12,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [1U], 1U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg[2U] 
        = (0x0ff8U & VL_SHIFTL_III(12,12,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [2U], 1U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg[3U] 
        = (0x0ff0U & VL_SHIFTL_III(12,12,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [3U], 1U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg[4U] 
        = (0x0fe0U & VL_SHIFTL_III(12,12,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [4U], 1U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg[5U] 
        = (0x0fc0U & VL_SHIFTL_III(12,12,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [5U], 1U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg[6U] 
        = (0x0f80U & VL_SHIFTL_III(12,12,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [6U], 1U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg[7U] 
        = (0x0f00U & VL_SHIFTL_III(12,12,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [7U], 1U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg[1U] 
        = (0x00000fffU & ((IData)(1U) + ((0x0ffcU & 
                                          VL_SHIFTL_III(12,12,32, 
                                                        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                                        [1U], 1U)) 
                                         + (1U & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                            [1U]))));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg[2U] 
        = (0x00000fffU & ((IData)(2U) + ((0x0ff8U & 
                                          VL_SHIFTL_III(12,12,32, 
                                                        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                                        [2U], 1U)) 
                                         + (3U & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                            [2U]))));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg[3U] 
        = (0x00000fffU & ((IData)(4U) + ((0x0ff0U & 
                                          VL_SHIFTL_III(12,12,32, 
                                                        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                                        [3U], 1U)) 
                                         + (7U & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                            [3U]))));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg[4U] 
        = (0x00000fffU & ((IData)(8U) + ((0x0fe0U & 
                                          VL_SHIFTL_III(12,12,32, 
                                                        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                                        [4U], 1U)) 
                                         + (0x000fU 
                                            & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                            [4U]))));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg[5U] 
        = (0x00000fffU & ((IData)(0x0010U) + ((0x0fc0U 
                                               & VL_SHIFTL_III(12,12,32, 
                                                               vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                                               [5U], 1U)) 
                                              + (0x001fU 
                                                 & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                                 [5U]))));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg[6U] 
        = (0x00000fffU & ((IData)(0x0020U) + ((0x0f80U 
                                               & VL_SHIFTL_III(12,12,32, 
                                                               vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                                               [6U], 1U)) 
                                              + (0x003fU 
                                                 & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                                 [6U]))));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg[7U] 
        = (0x00000fffU & ((IData)(0x0040U) + ((0x0f00U 
                                               & VL_SHIFTL_III(12,12,32, 
                                                               vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                                               [7U], 1U)) 
                                              + (0x007fU 
                                                 & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                                 [7U]))));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg[1U] 
        = (0x0040U & VL_SHIFTL_III(11,11,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [1U], 6U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg[2U] 
        = (0x0060U & VL_SHIFTL_III(11,11,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [2U], 5U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg[3U] 
        = (0x0070U & VL_SHIFTL_III(11,11,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [3U], 4U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg[4U] 
        = (0x0078U & VL_SHIFTL_III(11,11,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [4U], 3U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg[5U] 
        = (0x007cU & VL_SHIFTL_III(11,11,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [5U], 2U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg[6U] 
        = (0x007eU & VL_SHIFTL_III(11,11,32, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                   [6U], 1U));
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg[7U] 
        = (0x007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
           [7U]);
}

VL_ATTR_COLD void Vfft_processor_tb___024root___eval_stl(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_stl\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vfft_processor_tb___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vfft_processor_tb___024root___eval_phase__stl(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_phase__stl\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vfft_processor_tb___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vfft_processor_tb___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vfft_processor_tb___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vfft_processor_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vfft_processor_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vfft_processor_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge fft_processor_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge fft_processor_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vfft_processor_tb___024root___ctor_var_reset(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___ctor_var_reset\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->fft_processor_tb__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5296059427080310023ull);
    vlSelf->fft_processor_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10277374299484408519ull);
    vlSelf->fft_processor_tb__DOT__real_in = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 1251479770381552405ull);
    vlSelf->fft_processor_tb__DOT__imag_in = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2970656230690605973ull);
    vlSelf->fft_processor_tb__DOT__data_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9047008424932944196ull);
    vlSelf->fft_processor_tb__DOT__sample_count = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 2521005802210127202ull);
    vlSelf->fft_processor_tb__DOT__fft_started = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6477802978680318629ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__sample_counter = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 4329327955551472628ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__bit_reversed_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 2475834577978985816ull);
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__input_real[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 8234872067841752336ull);
    }
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__input_imag[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 5735548211685189431ull);
    }
    for (int __Vi0 = 0; __Vi0 < 128; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__twiddle_real[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3889091525077233876ull);
    }
    for (int __Vi0 = 0; __Vi0 < 128; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__twiddle_imag[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8186244925431309738ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        for (int __Vi1 = 0; __Vi1 < 256; ++__Vi1) {
            vlSelf->fft_processor_tb__DOT__dut__DOT__fft_buffer_real[__Vi0][__Vi1] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2652888011373252445ull);
        }
    }
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        for (int __Vi1 = 0; __Vi1 < 256; ++__Vi1) {
            vlSelf->fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[__Vi0][__Vi1] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 7920725035380671478ull);
        }
    }
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_valid = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11661823872075375450ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__stage_counter[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 1536560467886417436ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__butterfly_size_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14841253284136488713ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__butterfly_index_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 7038570802439684415ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__group_index_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 17459185557593224441ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__upper_index_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 12528189513588916651ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__twiddle_index_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 14053831705617396697ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__a_r_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 13044250057636170050ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__a_i_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 8666258289740639617ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__b_r_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 13889931515951118185ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__b_i_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 17260366430914181648ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[__Vi0] = VL_SCOPED_RAND_RESET_Q(48, __VscopeHash, 10087204190528000193ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[__Vi0] = VL_SCOPED_RAND_RESET_Q(48, __VscopeHash, 18297495690234692027ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__output_real_top_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 10169781561455483820ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2979323349197809614ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 9569200795227609125ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2540006044643275718ull);
    }
    vlSelf->fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum = VL_SCOPED_RAND_RESET_I(25, __VscopeHash, 9277280162203909342ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__output_counter = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 980772787880498712ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__output_valid_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11894277787598298612ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__angle = 0;
    vlSelf->fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__cos_val = 0;
    vlSelf->fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__sin_val = 0;
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__twiddle_r_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11782308365535989890ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__twiddle_i_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11318405202279261381ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__b_r_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 9623922167858044087ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__b_i_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3438439216485096567ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk3__DOT__bw_real_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2236383616479400699ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk3__DOT__bw_imag_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 18444736976648278660ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__twiddle_r_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 12078540883251239373ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__twiddle_i_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10188131892773942349ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__b_r_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 17276104751455637912ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__b_i_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2734252866250975972ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk3__DOT__bw_real_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 691543445049241352ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk3__DOT__bw_imag_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 12366745091205878789ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__twiddle_r_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1412206046621513563ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__twiddle_i_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5769184974741438872ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__b_r_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 17960455008963731259ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__b_i_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 18000164008850530099ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk3__DOT__bw_real_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 6522001051742015210ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk3__DOT__bw_imag_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 15297475869020149287ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__twiddle_r_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17788307067896600949ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__twiddle_i_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15533350819050404498ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__b_r_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 5018035350007298701ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__b_i_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 7319588127062442330ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk3__DOT__bw_real_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 5267742410657711085ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk3__DOT__bw_imag_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 5141568526760379618ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__twiddle_r_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1101550160910836888ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__twiddle_i_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6947534810064287989ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__b_r_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 25243628644943111ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__b_i_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 10992222005606580105ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk3__DOT__bw_real_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 477323714655089654ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk3__DOT__bw_imag_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 13791308459183844149ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__twiddle_r_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3000835593882464517ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__twiddle_i_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7725199170726851636ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__b_r_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 8164961746008456085ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__b_i_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 5926273730978679735ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk3__DOT__bw_real_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 6088465653130405157ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk3__DOT__bw_imag_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 17181284226426236148ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__twiddle_r_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 12594162118519234643ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__twiddle_i_val = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4263699568766743789ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__b_r_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3367905723328531504ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__b_i_scaled = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 14440335175345901536ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk3__DOT__bw_real_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 5182457543513477629ull);
    vlSelf->fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk3__DOT__bw_imag_rounded = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 1414024747090638090ull);
    vlSelf->__Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 5546214380306983212ull);
    vlSelf->__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4872557634172811123ull);
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__input_real__v0 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 10028517747067507058ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__input_real__v0 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1382345321175687081ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__input_real__v0 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__input_imag__v0 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 7054872047807338423ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__input_imag__v0 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8086870651903385554ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__input_imag__v0 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 5211480491376946769ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4368975425389066605ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3797602321850567273ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6684744218074573141ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v0 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v1 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14731883005952937830ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v1 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v2 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v3 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v4 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 8710920598792904342ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v4 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 16207071285511420336ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4725427166665914430ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v1 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 6002212447234347865ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v1 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5886512040965688358ull);
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2458860488659874565ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 303655369777094302ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 9090699755026415630ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16839488909897505032ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v6 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v7 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 12437724545019722051ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v7 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 9705668431958038133ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12983922996057865147ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v3 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 8046192756141765569ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v3 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14752282407729854182ull);
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 6532660379326758163ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16228007223780827042ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 190696458861093391ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14761173302390988062ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v9 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v10 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 5875342827843897033ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v10 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3214501406428447015ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9658439597766738166ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v5 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3279183766653204199ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v5 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13275269056769894415ull);
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 5007135155099997882ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6575152909021513650ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 11187241784398476820ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2752514694071876228ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v12 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v13 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 13360303039202305842ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v13 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 15159842626584813077ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6286375418809354862ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v7 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 10142734785513793869ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v7 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8211501112341730275ull);
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 229289763988214895ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10373342969182033676ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 14801675678990776872ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16665297832003964499ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v15 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v16 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 10623238486528211934ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v16 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3343103400609645422ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4161446982747362287ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v9 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 18119021977373115645ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v9 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15867065654894978851ull);
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2007842757797894545ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5831259315172941670ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 6689764018926702455ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4110311697677686823ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v18 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v19 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6063712582621683829ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v19 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2205090829734591271ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4673362513330592365ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v11 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 17256479881153800667ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v11 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3716667815984228902ull);
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 14830653542033968946ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5469870601654408102ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 9207600887182539653ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12768363671688762622ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v21 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v22 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 833253584316049210ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v22 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 15697585950202181578ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14901395820750985579ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v13 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 13042028743318274575ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v13 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4492942632482329026ull);
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 11326209136785357629ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4442110327188475791ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14 = 0;
    vlSelf->__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14 = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 4848077654801829087ull);
    vlSelf->__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18159002018854046772ull);
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14 = 0;
    vlSelf->__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v23 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__fft_processor_tb__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12793176865775072487ull);
    vlSelf->__Vtrigprevexpr___TOP__fft_processor_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4369172758278972693ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
