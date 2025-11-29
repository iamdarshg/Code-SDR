// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vhamming_window_tb__pch.h"
#include "Vhamming_window_tb.h"
#include "Vhamming_window_tb___024root.h"

// FUNCTIONS
Vhamming_window_tb__Syms::~Vhamming_window_tb__Syms()
{
}

Vhamming_window_tb__Syms::Vhamming_window_tb__Syms(VerilatedContext* contextp, const char* namep, Vhamming_window_tb* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(218);
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-9);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
}
