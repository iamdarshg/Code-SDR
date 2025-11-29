// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vfft_processor_tb.h for the primary calling header

#include "Vfft_processor_tb__pch.h"

void Vfft_processor_tb___024root___ctor_var_reset(Vfft_processor_tb___024root* vlSelf);

Vfft_processor_tb___024root::Vfft_processor_tb___024root(Vfft_processor_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vfft_processor_tb___024root___ctor_var_reset(this);
}

void Vfft_processor_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vfft_processor_tb___024root::~Vfft_processor_tb___024root() {
}
