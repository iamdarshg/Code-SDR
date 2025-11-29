// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vaverage_power_detector_tb__pch.h"
#include "Vaverage_power_detector_tb.h"
#include "Vaverage_power_detector_tb___024root.h"

// FUNCTIONS
Vaverage_power_detector_tb__Syms::~Vaverage_power_detector_tb__Syms()
{
}

Vaverage_power_detector_tb__Syms::Vaverage_power_detector_tb__Syms(VerilatedContext* contextp, const char* namep, Vaverage_power_detector_tb* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(294);
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-9);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
}
