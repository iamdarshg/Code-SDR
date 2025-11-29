// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VHAMMING_WINDOW_TB__SYMS_H_
#define VERILATED_VHAMMING_WINDOW_TB__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vhamming_window_tb.h"

// INCLUDE MODULE CLASSES
#include "Vhamming_window_tb___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vhamming_window_tb__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vhamming_window_tb* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vhamming_window_tb___024root   TOP;

    // CONSTRUCTORS
    Vhamming_window_tb__Syms(VerilatedContext* contextp, const char* namep, Vhamming_window_tb* modelp);
    ~Vhamming_window_tb__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
