// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vrp2040_interface_tb.h for the primary calling header

#include "Vrp2040_interface_tb__pch.h"

void Vrp2040_interface_tb___024root___ctor_var_reset(Vrp2040_interface_tb___024root* vlSelf);

Vrp2040_interface_tb___024root::Vrp2040_interface_tb___024root(Vrp2040_interface_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vrp2040_interface_tb___024root___ctor_var_reset(this);
}

void Vrp2040_interface_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vrp2040_interface_tb___024root::~Vrp2040_interface_tb___024root() {
}
