// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vfft_processor_tb.h for the primary calling header

#include "Vfft_processor_tb__pch.h"

VL_ATTR_COLD void Vfft_processor_tb___024root___eval_initial__TOP(Vfft_processor_tb___024root* vlSelf);
VlCoroutine Vfft_processor_tb___024root___eval_initial__TOP__Vtiming__0(Vfft_processor_tb___024root* vlSelf);
VlCoroutine Vfft_processor_tb___024root___eval_initial__TOP__Vtiming__1(Vfft_processor_tb___024root* vlSelf);

void Vfft_processor_tb___024root___eval_initial(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_initial\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vfft_processor_tb___024root___eval_initial__TOP(vlSelf);
    Vfft_processor_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vfft_processor_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vfft_processor_tb___024root___eval_initial__TOP__Vtiming__0(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.fft_processor_tb__DOT__clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "verilog/fft_processor_tb.v", 
                                             45);
        vlSelfRef.fft_processor_tb__DOT__clk = (1U 
                                                & (~ (IData)(vlSelfRef.fft_processor_tb__DOT__clk)));
    }
}

VlCoroutine Vfft_processor_tb___024root___eval_initial__TOP__Vtiming__1(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ fft_processor_tb__DOT__unnamedblk1_1__DOT____Vrepeat0;
    fft_processor_tb__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    // Body
    vlSelfRef.fft_processor_tb__DOT__sample_count = 0U;
    vlSelfRef.fft_processor_tb__DOT__fft_started = 0U;
    vlSelfRef.fft_processor_tb__DOT__rst_n = 0U;
    vlSelfRef.fft_processor_tb__DOT__real_in = 0U;
    vlSelfRef.fft_processor_tb__DOT__imag_in = 0U;
    vlSelfRef.fft_processor_tb__DOT__data_valid = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/fft_processor_tb.v", 
                                         63);
    vlSelfRef.fft_processor_tb__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/fft_processor_tb.v", 
                                         67);
    fft_processor_tb__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000100U;
    while (VL_LTS_III(32, 0U, fft_processor_tb__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        co_await vlSelfRef.__VtrigSched_h7903b55b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge fft_processor_tb.clk)", 
                                                             "verilog/fft_processor_tb.v", 
                                                             71);
        vlSelfRef.fft_processor_tb__DOT__real_in = 
            (0x00ffffffU & VL_MODDIVS_III(32, (IData)(
                                                      VL_RANDOM_I()), (IData)(0x00400000U)));
        vlSelfRef.fft_processor_tb__DOT__imag_in = 
            (0x00ffffffU & VL_MODDIVS_III(32, (IData)(
                                                      VL_RANDOM_I()), (IData)(0x00400000U)));
        vlSelfRef.fft_processor_tb__DOT__data_valid = 1U;
        fft_processor_tb__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (fft_processor_tb__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VtrigSched_h7903b55b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge fft_processor_tb.clk)", 
                                                         "verilog/fft_processor_tb.v", 
                                                         77);
    vlSelfRef.fft_processor_tb__DOT__data_valid = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x000000003b9aca00ULL, 
                                         nullptr, "verilog/fft_processor_tb.v", 
                                         81);
    if (vlSelfRef.fft_processor_tb__DOT__fft_started) {
        VL_WRITEF_NX("TEST PASSED: FFT processing completed and outputs generated\n",0);
    } else {
        VL_WRITEF_NX("TEST FAILED: FFT processing did not start or no outputs\n",0);
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "verilog/fft_processor_tb.v", 
                                         90);
    VL_FINISH_MT("verilog/fft_processor_tb.v", 91, "");
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vfft_processor_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vfft_processor_tb___024root___eval_triggers__act(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_triggers__act\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 2U) 
                                                     | ((((~ (IData)(vlSelfRef.fft_processor_tb__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__fft_processor_tb__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.fft_processor_tb__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__fft_processor_tb__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__fft_processor_tb__DOT__clk__0 
        = vlSelfRef.fft_processor_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__fft_processor_tb__DOT__rst_n__0 
        = vlSelfRef.fft_processor_tb__DOT__rst_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vfft_processor_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vfft_processor_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___trigger_anySet__act\n"); );
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

void Vfft_processor_tb___024root___nba_sequent__TOP__0(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___nba_sequent__TOP__0\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vdly__fft_processor_tb__DOT__fft_started;
    __Vdly__fft_processor_tb__DOT__fft_started = 0;
    CData/*0:0*/ __VdlyMask__fft_processor_tb__DOT__fft_started;
    __VdlyMask__fft_processor_tb__DOT__fft_started = 0;
    // Body
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__input_real__v0 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__input_imag__v0 = 0U;
    if (vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_valid_reg) {
        if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.fft_processor_tb__DOT__fft_started)))))) {
            VL_WRITEF_NX("FFT processing started, first valid output at time %t\n",0,
                         64,VL_TIME_UNITED_Q(1000),
                         -9);
            __Vdly__fft_processor_tb__DOT__fft_started = 1U;
            __VdlyMask__fft_processor_tb__DOT__fft_started = 1U;
        }
        vlSelfRef.fft_processor_tb__DOT__sample_count 
            = ((IData)(1U) + vlSelfRef.fft_processor_tb__DOT__sample_count);
        if (VL_UNLIKELY((VL_LTS_III(32, 0x00000100U, vlSelfRef.fft_processor_tb__DOT__sample_count)))) {
            VL_WRITEF_NX("Warning: More samples than expected\n",0);
        }
    }
    if (vlSelfRef.fft_processor_tb__DOT__data_valid) {
        vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__input_real__v0 
            = vlSelfRef.fft_processor_tb__DOT__real_in;
        vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__input_real__v0 
            = (0x000000ffU & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__bit_reversed_addr));
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__input_real__v0 = 1U;
        vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__input_imag__v0 
            = vlSelfRef.fft_processor_tb__DOT__imag_in;
        vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__input_imag__v0 
            = (0x000000ffU & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__bit_reversed_addr));
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__input_imag__v0 = 1U;
    }
    vlSelfRef.fft_processor_tb__DOT__fft_started = 
        (((IData)(__Vdly__fft_processor_tb__DOT__fft_started) 
          & (IData)(__VdlyMask__fft_processor_tb__DOT__fft_started)) 
         | ((IData)(vlSelfRef.fft_processor_tb__DOT__fft_started) 
            & (~ (IData)(__VdlyMask__fft_processor_tb__DOT__fft_started))));
    __VdlyMask__fft_processor_tb__DOT__fft_started = 0U;
}

void Vfft_processor_tb___024root___nba_sequent__TOP__1(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___nba_sequent__TOP__1\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0;
    fft_processor_tb__DOT__dut__DOT____Vlvbound_h3ce95289__0 = 0;
    SData/*11:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__addr = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__b = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__Vfuncout;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__a;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__a = 0;
    IData/*23:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__b;
    __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__b = 0;
    SData/*11:0*/ __Vdly__fft_processor_tb__DOT__dut__DOT__sample_counter;
    __Vdly__fft_processor_tb__DOT__dut__DOT__sample_counter = 0;
    CData/*0:0*/ __Vdly__fft_processor_tb__DOT__dut__DOT__output_valid_reg;
    __Vdly__fft_processor_tb__DOT__dut__DOT__output_valid_reg = 0;
    SData/*11:0*/ __Vdly__fft_processor_tb__DOT__dut__DOT__output_counter;
    __Vdly__fft_processor_tb__DOT__dut__DOT__output_counter = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v0;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v0 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v0;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v0 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v0;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v0 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v0;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v0 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v0;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v0 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v0;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v0 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v0;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v0 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v0;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v0 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v0;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v0 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v0;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v0 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v0;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v0 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v0;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v0 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v0;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v0 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v0;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v0 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v0;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v0 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v0;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v0 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v0;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v0 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v2;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v2 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v2;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v2 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v2;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v2 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v2;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v2 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v2;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v2 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v2;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v2 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v2;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v2 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v2;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v2 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v2;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v2 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v2;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v2 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v2;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v2 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v2;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v2 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v2;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v2 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v2;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v2 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v2;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v2 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v2;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v2 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v2;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v2 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v4;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v4 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v4;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v4 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v4;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v4 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v4;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v4 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v4;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v4 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v4;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v4 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v4;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v4 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v4;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v4 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v4;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v4 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v4;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v4 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v4;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v4 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v4;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v4 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v4;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v4 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v4;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v4 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v4;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v4 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v4;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v4 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v4;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v4 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v6;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v6 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v6;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v6 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v6;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v6 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v6;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v6 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v6;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v6 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v6;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v6 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v6;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v6 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v6;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v6 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v6;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v6 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v6;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v6 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v6;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v6 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v6;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v6 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v6;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v6 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v6;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v6 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v6;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v6 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v6;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v6 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v6;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v6 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v8;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v8 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v8;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v8 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v8;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v8 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v8;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v8 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v8;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v8 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v8;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v8 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v8;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v8 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v8;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v8 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v8;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v8 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v8;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v8 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v8;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v8 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v8;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v8 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v8;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v8 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v8;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v8 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v8;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v8 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v8;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v8 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v8;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v8 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v10;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v10 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v10;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v10 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v10;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v10 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v10;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v10 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v10;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v10 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v10;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v10 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v10;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v10 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v10;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v10 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v10;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v10 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v10;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v10 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v10;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v10 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v10;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v10 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v10;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v10 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v10;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v10 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v10;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v10 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v10;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v10 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v10;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v10 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v12;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v12 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v12;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v12 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v12;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v12 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v12;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v12 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v12;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v12 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v12;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v12 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v12;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v12 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v12;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v12 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v12;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v12 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v12;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v12 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v12;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v12 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v12;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v12 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v12;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v12 = 0;
    IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v12;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v12 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v12;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v12 = 0;
    CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v12;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v12 = 0;
    QData/*47:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v12;
    __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v12 = 0;
    // Body
    __Vdly__fft_processor_tb__DOT__dut__DOT__sample_counter 
        = vlSelfRef.fft_processor_tb__DOT__dut__DOT__sample_counter;
    __Vdly__fft_processor_tb__DOT__dut__DOT__output_valid_reg 
        = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_valid_reg;
    __Vdly__fft_processor_tb__DOT__dut__DOT__output_counter 
        = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_counter;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v0 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v1 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v2 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v0 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v2 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v4 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v6 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v8 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v10 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v12 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v0 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v2 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v4 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v6 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v8 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v10 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v12 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v0 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v2 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v4 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v6 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v8 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v10 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v12 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v0 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v2 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v4 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v6 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v8 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v10 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v12 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v0 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v2 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v4 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v6 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v8 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v10 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v12 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v0 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v2 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v4 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v6 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v8 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v10 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v12 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v0 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v2 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v4 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v6 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v8 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v10 = 0U;
    __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v12 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v3 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v4 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v6 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v7 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v9 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v10 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v12 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v13 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v15 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v16 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v18 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v19 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v21 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v22 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20 = 0U;
    vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v23 = 0U;
    vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
        = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid;
    if (vlSelfRef.fft_processor_tb__DOT__rst_n) {
        if (vlSelfRef.fft_processor_tb__DOT__data_valid) {
            __Vdly__fft_processor_tb__DOT__dut__DOT__sample_counter 
                = (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__sample_counter)));
        }
        if (((0x00ffU == vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
              [7U]) & ((IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid) 
                       >> 7U))) {
            __Vdly__fft_processor_tb__DOT__dut__DOT__output_valid_reg = 1U;
            __Vdly__fft_processor_tb__DOT__dut__DOT__output_counter = 0U;
        } else if (vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_valid_reg) {
            __Vdly__fft_processor_tb__DOT__dut__DOT__output_counter 
                = (0x00000fffU & ((IData)(1U) + (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_counter)));
            if ((0x00ffU == (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_counter))) {
                __Vdly__fft_processor_tb__DOT__dut__DOT__output_valid_reg = 0U;
            }
        }
        if ((0x00ffU == (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__sample_counter))) {
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v0 = 1U;
            vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
                = (1U | (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
        } else if ((1U & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid))) {
            vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v1 
                = (0x00000fffU & ((IData)(1U) + vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                  [0U]));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v1 = 1U;
        }
        if (((0x00ffU == vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
              [0U]) & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid))) {
            vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
                = (2U | (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v3 = 1U;
        } else if ((2U & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid))) {
            vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v4 
                = (0x00000fffU & ((IData)(1U) + vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                  [1U]));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v4 = 1U;
            if ((vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                 [1U] < (0x00000fffU & VL_SHIFTR_III(12,12,32, 
                                                     vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg
                                                     [1U], 1U)))) {
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg
                    [1U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [1U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [1U]));
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1 = 1U;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v1 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg
                    [1U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v1 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [1U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [1U]));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__twiddle_r_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_real
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [1U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk3__DOT__bw_real_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg
                                                  [1U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__twiddle_i_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_imag
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [1U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk3__DOT__bw_imag_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg
                                                  [1U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__b_r_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg
                                                   [1U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__b_i_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg
                                                   [1U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [1U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v0 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__1__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v0 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [1U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v0 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__2__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v0 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [1U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v0 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [0U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [1U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [1U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v0 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v0 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [0U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [1U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v0 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__4__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg
                    [1U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [1U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v0 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__3__Vfuncout;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [1U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v0 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [0U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [1U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [1U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v0 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v0 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [0U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [1U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v0 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__6__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg
                    [1U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [1U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v0 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__5__Vfuncout;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v0 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__twiddle_r_val)))) 
                                                - VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__twiddle_i_val))))));
                __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v0 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v0 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__twiddle_i_val)))) 
                                                + VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__twiddle_r_val))))));
            }
        }
        if (((0x00ffU == vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
              [1U]) & ((IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid) 
                       >> 1U))) {
            vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
                = (4U | (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v6 = 1U;
        } else if ((4U & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid))) {
            vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v7 
                = (0x00000fffU & ((IData)(1U) + vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                  [2U]));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v7 = 1U;
            if ((vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                 [2U] < (0x00000fffU & VL_SHIFTR_III(12,12,32, 
                                                     vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg
                                                     [2U], 1U)))) {
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg
                    [2U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [2U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [2U]));
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3 = 1U;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v3 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg
                    [2U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v3 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [2U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [2U]));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__twiddle_r_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_real
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [2U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk3__DOT__bw_real_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg
                                                  [2U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__twiddle_i_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_imag
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [2U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk3__DOT__bw_imag_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg
                                                  [2U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__b_r_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg
                                                   [2U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__b_i_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg
                                                   [2U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [2U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v2 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__7__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v2 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [2U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v2 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__8__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v2 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [2U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v2 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [1U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [2U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [2U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v2 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v2 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [1U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [2U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v2 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__10__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg
                    [2U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [2U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v2 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__9__Vfuncout;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [2U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v2 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [1U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [2U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [2U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v2 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v2 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [1U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [2U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v2 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__12__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg
                    [2U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [2U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v2 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__11__Vfuncout;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v2 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__twiddle_r_val)))) 
                                                - VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__twiddle_i_val))))));
                __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v2 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v2 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__twiddle_i_val)))) 
                                                + VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__twiddle_r_val))))));
            }
        }
        if (((0x00ffU == vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
              [2U]) & ((IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid) 
                       >> 2U))) {
            vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
                = (8U | (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v9 = 1U;
        } else if ((8U & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid))) {
            vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v10 
                = (0x00000fffU & ((IData)(1U) + vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                  [3U]));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v10 = 1U;
            if ((vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                 [3U] < (0x00000fffU & VL_SHIFTR_III(12,12,32, 
                                                     vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg
                                                     [3U], 1U)))) {
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg
                    [3U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [3U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [3U]));
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5 = 1U;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v5 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg
                    [3U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v5 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [3U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [3U]));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__twiddle_r_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_real
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [3U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk3__DOT__bw_real_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg
                                                  [3U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__twiddle_i_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_imag
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [3U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk3__DOT__bw_imag_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg
                                                  [3U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__b_r_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg
                                                   [3U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__b_i_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg
                                                   [3U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [3U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v4 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__13__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v4 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [3U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v4 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__14__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v4 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [3U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v4 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [0U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [3U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [3U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v4 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v4 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [0U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [3U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v4 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__16__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg
                    [3U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [3U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v4 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__15__Vfuncout;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [3U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v4 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [0U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [3U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [3U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v4 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v4 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [0U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [3U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v4 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__18__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg
                    [3U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [3U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v4 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__17__Vfuncout;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v4 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__twiddle_r_val)))) 
                                                - VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__twiddle_i_val))))));
                __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v4 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v4 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__twiddle_i_val)))) 
                                                + VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__twiddle_r_val))))));
            }
        }
        if (((0x00ffU == vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
              [3U]) & ((IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid) 
                       >> 3U))) {
            vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
                = (0x00000010U | (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v12 = 1U;
        } else if ((0x00000010U & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid))) {
            vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v13 
                = (0x00000fffU & ((IData)(1U) + vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                  [4U]));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v13 = 1U;
            if ((vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                 [4U] < (0x00000fffU & VL_SHIFTR_III(12,12,32, 
                                                     vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg
                                                     [4U], 1U)))) {
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg
                    [4U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [4U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [4U]));
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7 = 1U;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v7 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg
                    [4U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v7 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [4U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [4U]));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__twiddle_r_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_real
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [4U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk3__DOT__bw_real_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg
                                                  [4U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__twiddle_i_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_imag
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [4U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk3__DOT__bw_imag_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg
                                                  [4U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__b_r_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg
                                                   [4U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__b_i_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg
                                                   [4U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [4U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v6 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__19__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v6 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [4U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v6 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__20__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v6 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [4U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v6 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [1U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [4U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [4U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v6 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v6 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [1U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [4U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v6 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__22__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg
                    [4U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [4U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v6 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__21__Vfuncout;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [4U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v6 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [1U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [4U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [4U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v6 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v6 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [1U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [4U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v6 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__24__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg
                    [4U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [4U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v6 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__23__Vfuncout;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v6 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__twiddle_r_val)))) 
                                                - VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__twiddle_i_val))))));
                __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v6 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v6 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__twiddle_i_val)))) 
                                                + VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__twiddle_r_val))))));
            }
        }
        if (((0x00ffU == vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
              [4U]) & ((IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid) 
                       >> 4U))) {
            vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
                = (0x00000020U | (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v15 = 1U;
        } else if ((0x00000020U & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid))) {
            vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v16 
                = (0x00000fffU & ((IData)(1U) + vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                  [5U]));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v16 = 1U;
            if ((vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                 [5U] < (0x00000fffU & VL_SHIFTR_III(12,12,32, 
                                                     vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg
                                                     [5U], 1U)))) {
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg
                    [5U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [5U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [5U]));
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9 = 1U;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v9 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg
                    [5U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v9 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [5U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [5U]));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__twiddle_r_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_real
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [5U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk3__DOT__bw_real_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg
                                                  [5U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__twiddle_i_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_imag
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [5U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk3__DOT__bw_imag_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg
                                                  [5U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__b_r_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg
                                                   [5U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__b_i_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg
                                                   [5U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [5U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v8 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__25__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v8 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [5U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v8 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__26__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v8 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [5U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v8 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [0U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [5U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [5U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v8 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v8 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [0U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [5U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v8 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__28__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg
                    [5U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [5U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v8 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__27__Vfuncout;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [5U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v8 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [0U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [5U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [5U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v8 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v8 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [0U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [5U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v8 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__30__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg
                    [5U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [5U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v8 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__29__Vfuncout;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v8 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__twiddle_r_val)))) 
                                                - VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__twiddle_i_val))))));
                __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v8 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v8 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__twiddle_i_val)))) 
                                                + VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__twiddle_r_val))))));
            }
        }
        if (((0x00ffU == vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
              [5U]) & ((IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid) 
                       >> 5U))) {
            vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
                = (0x00000040U | (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v18 = 1U;
        } else if ((0x00000040U & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid))) {
            vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v19 
                = (0x00000fffU & ((IData)(1U) + vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                  [6U]));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v19 = 1U;
            if ((vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                 [6U] < (0x00000fffU & VL_SHIFTR_III(12,12,32, 
                                                     vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg
                                                     [6U], 1U)))) {
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg
                    [6U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [6U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [6U]));
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11 = 1U;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v11 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg
                    [6U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v11 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [6U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [6U]));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__twiddle_r_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_real
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [6U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk3__DOT__bw_real_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg
                                                  [6U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__twiddle_i_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_imag
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [6U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk3__DOT__bw_imag_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg
                                                  [6U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__b_r_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg
                                                   [6U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__b_i_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg
                                                   [6U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [6U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v10 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__31__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v10 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [6U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v10 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__32__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v10 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [6U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v10 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [1U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [6U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [6U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v10 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v10 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [1U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [6U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v10 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__34__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg
                    [6U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [6U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v10 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__33__Vfuncout;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [6U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v10 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [1U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [6U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [6U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v10 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v10 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [1U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [6U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v10 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__36__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg
                    [6U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [6U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v10 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__35__Vfuncout;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v10 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__twiddle_r_val)))) 
                                                - VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__twiddle_i_val))))));
                __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v10 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v10 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__twiddle_i_val)))) 
                                                + VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__twiddle_r_val))))));
            }
        }
        if (((0x00ffU == vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
              [6U]) & ((IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid) 
                       >> 6U))) {
            vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
                = (0x00000080U | (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v21 = 1U;
        } else if ((0x00000080U & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid))) {
            vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v22 
                = (0x00000fffU & ((IData)(1U) + vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
                                  [7U]));
            vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v22 = 1U;
            if ((vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                 [7U] < (0x00000fffU & VL_SHIFTR_III(12,12,32, 
                                                     vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_size_reg
                                                     [7U], 1U)))) {
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg
                    [7U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [7U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [7U]));
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13 = 1U;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v13 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg
                    [7U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v13 
                    = (0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                      [7U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                      [7U]));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__twiddle_r_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_real
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [7U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk3__DOT__bw_real_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg
                                                  [7U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__twiddle_i_val 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_imag
                    [(0x0000007fU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__twiddle_index_reg
                      [7U])];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk3__DOT__bw_imag_rounded 
                    = (0x00ffffffU & (IData)((0x000000000fffffffULL 
                                              & ((0x0000000000080000ULL 
                                                  + 
                                                  vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg
                                                  [7U]) 
                                                 >> 0x00000014U))));
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__b_r_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg
                                                   [7U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__b_i_scaled 
                    = (0x00ffffffU & VL_SHIFTR_III(24,24,32, 
                                                   vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg
                                                   [7U], 4U));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [7U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v12 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__37__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v12 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [7U];
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v12 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__38__Vfuncout;
                __VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v12 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg
                    [7U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v12 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [0U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [7U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [7U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v12 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v12 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real
                    [0U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [7U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v12 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__40__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg
                    [7U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [7U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v12 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__39__Vfuncout;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__b 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__a 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg
                    [7U];
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__b 
                    = (0x00ffffffU & ((IData)(1U) + 
                                      (~ __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__b)));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__a 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__a;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v12 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [0U][(0x000000ffU & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__group_index_reg
                                         [7U] + vlSelfRef.fft_processor_tb__DOT__dut__DOT__butterfly_index_reg
                                         [7U]))];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v12 = 1U;
                vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                    = (0x01ffffffU & (((0x01000000U 
                                        & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__a 
                                           << 1U)) 
                                       | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__a) 
                                      + ((0x01000000U 
                                          & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__b 
                                             << 1U)) 
                                         | __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__b)));
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v12 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag
                    [0U][(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                          [7U])];
                __VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v12 = 1U;
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__Vfuncout 
                    = (0x00ffffffU & ((((1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__a 
                                               >> 0x17U)) 
                                        == (1U & (__Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__b 
                                                  >> 0x17U))) 
                                       & ((1U & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                 >> 0x18U)) 
                                          != (1U & 
                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                               >> 0x17U))))
                                       ? ((0x00800000U 
                                           & (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                              >> 1U)) 
                                          | (0x007fffffU 
                                             & (- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum 
                                                               >> 0x18U)))))))
                                       : vlSelfRef.fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum));
                __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__Vfuncout 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_add__42__Vfuncout;
                vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14 
                    = vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg
                    [7U];
                vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14 
                    = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__upper_index_reg
                       [7U]);
                vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v12 
                    = __Vfunc_fft_processor_tb__DOT__dut__DOT__saturated_sub__41__Vfuncout;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v12 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__twiddle_r_val)))) 
                                                - VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__twiddle_i_val))))));
                __VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v12 = 1U;
                __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v12 
                    = (0x0000ffffffffffffULL & (VL_MULS_QQQ(48, 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__b_r_scaled)), 
                                                            (0x0000ffffffffffffULL 
                                                             & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__twiddle_i_val)))) 
                                                + VL_MULS_QQQ(48, 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,24, vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__b_i_scaled)), 
                                                              (0x0000ffffffffffffULL 
                                                               & VL_EXTENDS_QI(48,16, (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__twiddle_r_val))))));
            }
        }
    } else {
        __Vdly__fft_processor_tb__DOT__dut__DOT__sample_counter = 0U;
        __Vdly__fft_processor_tb__DOT__dut__DOT__output_counter = 0U;
        __Vdly__fft_processor_tb__DOT__dut__DOT__output_valid_reg = 0U;
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v2 = 1U;
        vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
            = (0xfeU & (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
        vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
            = (0xfdU & (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5 = 1U;
        vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
            = (0xfbU & (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8 = 1U;
        vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
            = (0xf7U & (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11 = 1U;
        vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
            = (0xefU & (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14 = 1U;
        vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
            = (0xdfU & (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17 = 1U;
        vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
            = (0xbfU & (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20 = 1U;
        vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid 
            = (0x7fU & (IData)(vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid));
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v23 = 1U;
    }
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_counter 
        = __Vdly__fft_processor_tb__DOT__dut__DOT__output_counter;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_valid_reg 
        = __Vdly__fft_processor_tb__DOT__dut__DOT__output_valid_reg;
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__sample_counter 
        = __Vdly__fft_processor_tb__DOT__dut__DOT__sample_counter;
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[1U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[1U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[2U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[2U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[3U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[3U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[4U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[4U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[5U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[5U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[6U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[6U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[7U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_top_reg__v12;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[1U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[1U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[2U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[2U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[3U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[3U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[4U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[4U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[5U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[5U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[6U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[6U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[7U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_top_reg__v12;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[1U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[1U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[2U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[2U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[3U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[3U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[4U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[4U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[5U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[5U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[6U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[6U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_r_reg__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[7U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_r_reg__v12;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[1U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[1U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[2U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[2U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[3U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[3U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[4U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[4U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[5U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[5U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[6U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[6U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_r_reg__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[7U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_r_reg__v12;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[1U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[1U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[2U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[2U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[3U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[3U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[4U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[4U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[5U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[5U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[6U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[6U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[7U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg__v12;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[1U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[1U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[2U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[2U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[3U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[3U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[4U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[4U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[5U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[5U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[6U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[6U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__a_i_reg__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[7U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__a_i_reg__v12;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[1U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[1U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[2U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[2U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[3U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[3U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[4U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[4U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[5U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[5U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[6U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[6U] = 0U;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__b_i_reg__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[7U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__b_i_reg__v12;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[1U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[1U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[2U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[2U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[3U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[3U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[4U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[4U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[5U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[5U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[6U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[6U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[7U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg__v12;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[1U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v0;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[1U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[1U] = 0ULL;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[1U] = 0ULL;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[2U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v2;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[2U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[2U] = 0ULL;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[2U] = 0ULL;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[3U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v4;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[3U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[3U] = 0ULL;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[3U] = 0ULL;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[4U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v6;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[4U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[4U] = 0ULL;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[4U] = 0ULL;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[5U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v8;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[5U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[5U] = 0ULL;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[5U] = 0ULL;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[6U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v10;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[6U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[6U] = 0ULL;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[6U] = 0ULL;
    }
    if (__VdlySet__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[7U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_real_reg__v12;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[7U] 
            = __VdlyVal__fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg__v12;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v23) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_top_reg[7U] = 0U;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_top_reg[7U] = 0U;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_r_reg[7U] = 0U;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_r_reg[7U] = 0U;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg[7U] = 0U;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__a_i_reg[7U] = 0U;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__b_i_reg[7U] = 0U;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg[7U] = 0U;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_real_reg[7U] = 0ULL;
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg[7U] = 0ULL;
    }
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
}

void Vfft_processor_tb___024root___nba_sequent__TOP__2(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___nba_sequent__TOP__2\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (IData)(vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid))) {
        vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0 
            = vlSelfRef.fft_processor_tb__DOT__dut__DOT__input_real
            [(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
              [0U])];
        vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0 
            = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
               [0U]);
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0 = 1U;
        vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0 
            = vlSelfRef.fft_processor_tb__DOT__dut__DOT__input_imag
            [(0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
              [0U])];
        vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0 
            = (0x000000ffU & vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter
               [0U]);
        vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0 = 1U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__input_real__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__input_real[vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__input_real__v0] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__input_real__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__input_imag__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__input_imag[vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__input_imag__v0] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__input_imag__v0;
    }
}

void Vfft_processor_tb___024root___nba_sequent__TOP__3(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___nba_sequent__TOP__3\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_real[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14;
    }
    vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_valid 
        = vlSelfRef.__Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid;
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v1] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v1;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v3] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v3;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v5] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v5;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v7] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v7;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v9] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v9;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v11] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v11;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[0U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v13] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v13;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__fft_buffer_imag[1U][vlSelfRef.__VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v0) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[0U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v1) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[0U] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v1;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v2) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[0U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v3) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[1U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v4) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[1U] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v4;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[1U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v6) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[2U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v7) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[2U] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v7;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[2U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v9) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[3U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v10) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[3U] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v10;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[3U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v12) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[4U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v13) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[4U] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v13;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[4U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v15) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[5U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v16) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[5U] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v16;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[5U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v18) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[6U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v19) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[6U] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v19;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[6U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v21) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[7U] = 0U;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v22) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[7U] 
            = vlSelfRef.__VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v22;
    }
    if (vlSelfRef.__VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v23) {
        vlSelfRef.fft_processor_tb__DOT__dut__DOT__stage_counter[7U] = 0U;
    }
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

void Vfft_processor_tb___024root___eval_nba(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_nba\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vfft_processor_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vfft_processor_tb___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vfft_processor_tb___024root___nba_sequent__TOP__2(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vfft_processor_tb___024root___nba_sequent__TOP__3(vlSelf);
    }
}

void Vfft_processor_tb___024root___timing_commit(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___timing_commit\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((! (1ULL & vlSelfRef.__VactTriggered[0U]))) {
        vlSelfRef.__VtrigSched_h7903b55b__0.commit(
                                                   "@(posedge fft_processor_tb.clk)");
    }
}

void Vfft_processor_tb___024root___timing_resume(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___timing_resume\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h7903b55b__0.resume(
                                                   "@(posedge fft_processor_tb.clk)");
    }
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vfft_processor_tb___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vfft_processor_tb___024root___eval_phase__act(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_phase__act\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vfft_processor_tb___024root___eval_triggers__act(vlSelf);
    Vfft_processor_tb___024root___timing_commit(vlSelf);
    Vfft_processor_tb___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vfft_processor_tb___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        Vfft_processor_tb___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

void Vfft_processor_tb___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vfft_processor_tb___024root___eval_phase__nba(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_phase__nba\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vfft_processor_tb___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vfft_processor_tb___024root___eval_nba(vlSelf);
        Vfft_processor_tb___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vfft_processor_tb___024root___eval(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vfft_processor_tb___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("verilog/fft_processor_tb.v", 3, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vfft_processor_tb___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("verilog/fft_processor_tb.v", 3, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vfft_processor_tb___024root___eval_phase__act(vlSelf));
    } while (Vfft_processor_tb___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vfft_processor_tb___024root___eval_debug_assertions(Vfft_processor_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vfft_processor_tb___024root___eval_debug_assertions\n"); );
    Vfft_processor_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
