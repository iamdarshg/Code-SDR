// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VADAPTIVE_GAIN_SCALER_TB__SYMS_H_
#define VERILATED_VADAPTIVE_GAIN_SCALER_TB__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vadaptive_gain_scaler_tb.h"

// INCLUDE MODULE CLASSES
#include "Vadaptive_gain_scaler_tb___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vadaptive_gain_scaler_tb__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vadaptive_gain_scaler_tb* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vadaptive_gain_scaler_tb___024root TOP;

    // CONSTRUCTORS
    Vadaptive_gain_scaler_tb__Syms(VerilatedContext* contextp, const char* namep, Vadaptive_gain_scaler_tb* modelp);
    ~Vadaptive_gain_scaler_tb__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
