// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VRP2040_INTERFACE_TB__SYMS_H_
#define VERILATED_VRP2040_INTERFACE_TB__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vrp2040_interface_tb.h"

// INCLUDE MODULE CLASSES
#include "Vrp2040_interface_tb___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vrp2040_interface_tb__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vrp2040_interface_tb* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vrp2040_interface_tb___024root TOP;

    // CONSTRUCTORS
    Vrp2040_interface_tb__Syms(VerilatedContext* contextp, const char* namep, Vrp2040_interface_tb* modelp);
    ~Vrp2040_interface_tb__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
