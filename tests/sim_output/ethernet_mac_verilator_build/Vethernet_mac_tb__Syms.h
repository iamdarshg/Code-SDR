// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VETHERNET_MAC_TB__SYMS_H_
#define VERILATED_VETHERNET_MAC_TB__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vethernet_mac_tb.h"

// INCLUDE MODULE CLASSES
#include "Vethernet_mac_tb___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vethernet_mac_tb__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vethernet_mac_tb* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vethernet_mac_tb___024root     TOP;

    // CONSTRUCTORS
    Vethernet_mac_tb__Syms(VerilatedContext* contextp, const char* namep, Vethernet_mac_tb* modelp);
    ~Vethernet_mac_tb__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
