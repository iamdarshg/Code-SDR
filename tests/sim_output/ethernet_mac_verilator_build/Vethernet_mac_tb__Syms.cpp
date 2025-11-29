// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vethernet_mac_tb__pch.h"
#include "Vethernet_mac_tb.h"
#include "Vethernet_mac_tb___024root.h"

// FUNCTIONS
Vethernet_mac_tb__Syms::~Vethernet_mac_tb__Syms()
{
}

Vethernet_mac_tb__Syms::Vethernet_mac_tb__Syms(VerilatedContext* contextp, const char* namep, Vethernet_mac_tb* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(422);
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-9);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
}
