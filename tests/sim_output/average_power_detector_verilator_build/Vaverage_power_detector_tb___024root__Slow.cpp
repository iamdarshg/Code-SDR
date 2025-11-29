// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vaverage_power_detector_tb.h for the primary calling header

#include "Vaverage_power_detector_tb__pch.h"

void Vaverage_power_detector_tb___024root___ctor_var_reset(Vaverage_power_detector_tb___024root* vlSelf);

Vaverage_power_detector_tb___024root::Vaverage_power_detector_tb___024root(Vaverage_power_detector_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vaverage_power_detector_tb___024root___ctor_var_reset(this);
}

void Vaverage_power_detector_tb___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vaverage_power_detector_tb___024root::~Vaverage_power_detector_tb___024root() {
}
