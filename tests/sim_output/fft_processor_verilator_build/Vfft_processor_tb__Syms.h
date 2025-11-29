// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VFFT_PROCESSOR_TB__SYMS_H_
#define VERILATED_VFFT_PROCESSOR_TB__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vfft_processor_tb.h"

// INCLUDE MODULE CLASSES
#include "Vfft_processor_tb___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vfft_processor_tb__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vfft_processor_tb* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vfft_processor_tb___024root    TOP;

    // CONSTRUCTORS
    Vfft_processor_tb__Syms(VerilatedContext* contextp, const char* namep, Vfft_processor_tb* modelp);
    ~Vfft_processor_tb__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
