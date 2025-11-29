// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vadc_interface_tb.h for the primary calling header

#include "Vadc_interface_tb__pch.h"

void Vadc_interface_tb___024root___ctor_var_reset(Vadc_interface_tb___024root* vlSelf);

Vadc_interface_tb___024root::Vadc_interface_tb___024root(Vadc_interface_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vadc_interface_tb___024root___ctor_var_reset(this);
}

void Vadc_interface_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vadc_interface_tb___024root::~Vadc_interface_tb___024root() {
}
