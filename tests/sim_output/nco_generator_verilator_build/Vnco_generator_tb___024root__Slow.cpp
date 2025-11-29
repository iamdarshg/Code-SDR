// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vnco_generator_tb.h for the primary calling header

#include "Vnco_generator_tb__pch.h"

void Vnco_generator_tb___024root___ctor_var_reset(Vnco_generator_tb___024root* vlSelf);

Vnco_generator_tb___024root::Vnco_generator_tb___024root(Vnco_generator_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vnco_generator_tb___024root___ctor_var_reset(this);
}

void Vnco_generator_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vnco_generator_tb___024root::~Vnco_generator_tb___024root() {
}
