// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vnco_generator_tb.h for the primary calling header

#include "Vnco_generator_tb__pch.h"

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_static(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_static\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__nco_generator_tb__DOT__clk__0 
        = vlSelfRef.nco_generator_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__nco_generator_tb__DOT__rst_n__0 
        = vlSelfRef.nco_generator_tb__DOT__rst_n;
}

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_initial__TOP(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_initial__TOP\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ nco_generator_tb__DOT__dut__DOT__rom_init_i;
    nco_generator_tb__DOT__dut__DOT__rom_init_i = 0;
    // Body
    nco_generator_tb__DOT__dut__DOT__rom_init_i = 0U;
    while (VL_GTS_III(32, 0x00000100U, nco_generator_tb__DOT__dut__DOT__rom_init_i)) {
        vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[(0x000000ffU 
                                                            & nco_generator_tb__DOT__dut__DOT__rom_init_i)] 
            = (0x0000ffffU & VL_RTOI_I_D((3.27670000000000000e+04 
                                          * sin(((3.14159265358979312e+00 
                                                  * 
                                                  VL_ISTOR_D_I(32, nco_generator_tb__DOT__dut__DOT__rom_init_i)) 
                                                 / 512.0)))));
        vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[(0x000000ffU 
                                                            & nco_generator_tb__DOT__dut__DOT__rom_init_i)] 
            = (0x0000ffffU & VL_RTOI_I_D((3.27670000000000000e+04 
                                          * cos(((3.14159265358979312e+00 
                                                  * 
                                                  VL_ISTOR_D_I(32, nco_generator_tb__DOT__dut__DOT__rom_init_i)) 
                                                 / 512.0)))));
        nco_generator_tb__DOT__dut__DOT__rom_init_i 
            = ((IData)(1U) + nco_generator_tb__DOT__dut__DOT__rom_init_i);
    }
}

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_final(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_final\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vnco_generator_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vnco_generator_tb___024root___eval_phase__stl(Vnco_generator_tb___024root* vlSelf);

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_settle(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_settle\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vnco_generator_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("verilog/nco_generator_tb.v", 3, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vnco_generator_tb___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_triggers__stl(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_triggers__stl\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vnco_generator_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vnco_generator_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vnco_generator_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vnco_generator_tb___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vnco_generator_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___trigger_anySet__stl\n"); );
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

void Vnco_generator_tb___024root___act_sequent__TOP__0(Vnco_generator_tb___024root* vlSelf);

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_stl(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_stl\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vnco_generator_tb___024root___act_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vnco_generator_tb___024root___eval_phase__stl(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_phase__stl\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vnco_generator_tb___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vnco_generator_tb___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vnco_generator_tb___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vnco_generator_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vnco_generator_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vnco_generator_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge nco_generator_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge nco_generator_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vnco_generator_tb___024root___ctor_var_reset(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___ctor_var_reset\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->nco_generator_tb__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7013447719023367527ull);
    vlSelf->nco_generator_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17040999325873965629ull);
    vlSelf->nco_generator_tb__DOT__frequency_word = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12459102510333918098ull);
    vlSelf->nco_generator_tb__DOT__enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4713346959820286104ull);
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->nco_generator_tb__DOT__dut__DOT__sin_rom[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7607983075232551432ull);
    }
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->nco_generator_tb__DOT__dut__DOT__cos_rom[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1255098502210238550ull);
    }
    vlSelf->nco_generator_tb__DOT__dut__DOT__phase_accumulator = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 16347092544274596520ull);
    vlSelf->nco_generator_tb__DOT__dut__DOT__phase_accumulator_next = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12379754452741059737ull);
    vlSelf->nco_generator_tb__DOT__dut__DOT__sine_reg = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16849890523114976822ull);
    vlSelf->nco_generator_tb__DOT__dut__DOT__cosine_reg = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8854082886331771150ull);
    vlSelf->nco_generator_tb__DOT__dut__DOT__valid_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10247284853561029685ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__nco_generator_tb__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11150424196796654784ull);
    vlSelf->__Vtrigprevexpr___TOP__nco_generator_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13158991576343744825ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
