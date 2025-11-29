// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vethernet_mac_tb.h for the primary calling header

#include "Vethernet_mac_tb__pch.h"

void Vethernet_mac_tb___024root___ctor_var_reset(Vethernet_mac_tb___024root* vlSelf);

Vethernet_mac_tb___024root::Vethernet_mac_tb___024root(Vethernet_mac_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vethernet_mac_tb___024root___ctor_var_reset(this);
}

void Vethernet_mac_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vethernet_mac_tb___024root::~Vethernet_mac_tb___024root() {
}
