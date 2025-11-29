// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VNCO_GENERATOR_TB__SYMS_H_
#define VERILATED_VNCO_GENERATOR_TB__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vnco_generator_tb.h"

// INCLUDE MODULE CLASSES
#include "Vnco_generator_tb___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vnco_generator_tb__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vnco_generator_tb* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vnco_generator_tb___024root    TOP;

    // CONSTRUCTORS
    Vnco_generator_tb__Syms(VerilatedContext* contextp, const char* namep, Vnco_generator_tb* modelp);
    ~Vnco_generator_tb__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
