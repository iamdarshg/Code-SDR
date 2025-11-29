// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vasync_fifo_tb.h for the primary calling header

#include "Vasync_fifo_tb__pch.h"

void Vasync_fifo_tb___024root___ctor_var_reset(Vasync_fifo_tb___024root* vlSelf);

Vasync_fifo_tb___024root::Vasync_fifo_tb___024root(Vasync_fifo_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vasync_fifo_tb___024root___ctor_var_reset(this);
}

void Vasync_fifo_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vasync_fifo_tb___024root::~Vasync_fifo_tb___024root() {
}
