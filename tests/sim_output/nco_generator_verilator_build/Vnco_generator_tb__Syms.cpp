// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vnco_generator_tb__pch.h"
#include "Vnco_generator_tb.h"
#include "Vnco_generator_tb___024root.h"

// FUNCTIONS
Vnco_generator_tb__Syms::~Vnco_generator_tb__Syms()
{
}

Vnco_generator_tb__Syms::Vnco_generator_tb__Syms(VerilatedContext* contextp, const char* namep, Vnco_generator_tb* modelp)
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
