// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vhamming_window_tb.h for the primary calling header

#include "Vhamming_window_tb__pch.h"

void Vhamming_window_tb___024root___ctor_var_reset(Vhamming_window_tb___024root* vlSelf);

Vhamming_window_tb___024root::Vhamming_window_tb___024root(Vhamming_window_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vhamming_window_tb___024root___ctor_var_reset(this);
}

void Vhamming_window_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vhamming_window_tb___024root::~Vhamming_window_tb___024root() {
}
