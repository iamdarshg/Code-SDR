// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vudp_ip_stack_tb.h for the primary calling header

#include "Vudp_ip_stack_tb__pch.h"

void Vudp_ip_stack_tb___024root___ctor_var_reset(Vudp_ip_stack_tb___024root* vlSelf);

Vudp_ip_stack_tb___024root::Vudp_ip_stack_tb___024root(Vudp_ip_stack_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vudp_ip_stack_tb___024root___ctor_var_reset(this);
}

void Vudp_ip_stack_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vudp_ip_stack_tb___024root::~Vudp_ip_stack_tb___024root() {
}
