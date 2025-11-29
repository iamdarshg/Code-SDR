// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VCIC_DECIMATOR_TB__SYMS_H_
#define VERILATED_VCIC_DECIMATOR_TB__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vcic_decimator_tb.h"

// INCLUDE MODULE CLASSES
#include "Vcic_decimator_tb___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vcic_decimator_tb__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vcic_decimator_tb* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vcic_decimator_tb___024root    TOP;

    // CONSTRUCTORS
    Vcic_decimator_tb__Syms(VerilatedContext* contextp, const char* namep, Vcic_decimator_tb* modelp);
    ~Vcic_decimator_tb__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
