// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vadaptive_gain_scaler_tb.h for the primary calling header

#include "Vadaptive_gain_scaler_tb__pch.h"

void Vadaptive_gain_scaler_tb___024root___ctor_var_reset(Vadaptive_gain_scaler_tb___024root* vlSelf);

Vadaptive_gain_scaler_tb___024root::Vadaptive_gain_scaler_tb___024root(Vadaptive_gain_scaler_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vadaptive_gain_scaler_tb___024root___ctor_var_reset(this);
}

void Vadaptive_gain_scaler_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vadaptive_gain_scaler_tb___024root::~Vadaptive_gain_scaler_tb___024root() {
}
