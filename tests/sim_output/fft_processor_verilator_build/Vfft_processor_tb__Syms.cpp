// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vfft_processor_tb__pch.h"
#include "Vfft_processor_tb.h"
#include "Vfft_processor_tb___024root.h"

// FUNCTIONS
Vfft_processor_tb__Syms::~Vfft_processor_tb__Syms()
{
}

Vfft_processor_tb__Syms::Vfft_processor_tb__Syms(VerilatedContext* contextp, const char* namep, Vfft_processor_tb* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(2026);
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-9);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
}
