// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vadaptive_gain_scaler_tb__pch.h"
#include "Vadaptive_gain_scaler_tb.h"
#include "Vadaptive_gain_scaler_tb___024root.h"

// FUNCTIONS
Vadaptive_gain_scaler_tb__Syms::~Vadaptive_gain_scaler_tb__Syms()
{
}

Vadaptive_gain_scaler_tb__Syms::Vadaptive_gain_scaler_tb__Syms(VerilatedContext* contextp, const char* namep, Vadaptive_gain_scaler_tb* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(262);
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-9);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
}
