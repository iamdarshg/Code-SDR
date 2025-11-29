// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vcic_decimator_tb.h for the primary calling header

#include "Vcic_decimator_tb__pch.h"

void Vcic_decimator_tb___024root___ctor_var_reset(Vcic_decimator_tb___024root* vlSelf);

Vcic_decimator_tb___024root::Vcic_decimator_tb___024root(Vcic_decimator_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vcic_decimator_tb___024root___ctor_var_reset(this);
}

void Vcic_decimator_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vcic_decimator_tb___024root::~Vcic_decimator_tb___024root() {
}
