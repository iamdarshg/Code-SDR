// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VADC_INTERFACE_TB__SYMS_H_
#define VERILATED_VADC_INTERFACE_TB__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vadc_interface_tb.h"

// INCLUDE MODULE CLASSES
#include "Vadc_interface_tb___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vadc_interface_tb__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vadc_interface_tb* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vadc_interface_tb___024root    TOP;

    // CONSTRUCTORS
    Vadc_interface_tb__Syms(VerilatedContext* contextp, const char* namep, Vadc_interface_tb* modelp);
    ~Vadc_interface_tb__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
