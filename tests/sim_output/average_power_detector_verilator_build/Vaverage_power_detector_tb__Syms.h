// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VAVERAGE_POWER_DETECTOR_TB__SYMS_H_
#define VERILATED_VAVERAGE_POWER_DETECTOR_TB__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vaverage_power_detector_tb.h"

// INCLUDE MODULE CLASSES
#include "Vaverage_power_detector_tb___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vaverage_power_detector_tb__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vaverage_power_detector_tb* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vaverage_power_detector_tb___024root TOP;

    // CONSTRUCTORS
    Vaverage_power_detector_tb__Syms(VerilatedContext* contextp, const char* namep, Vaverage_power_detector_tb* modelp);
    ~Vaverage_power_detector_tb__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
