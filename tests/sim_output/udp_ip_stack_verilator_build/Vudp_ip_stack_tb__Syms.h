// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VUDP_IP_STACK_TB__SYMS_H_
#define VERILATED_VUDP_IP_STACK_TB__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vudp_ip_stack_tb.h"

// INCLUDE MODULE CLASSES
#include "Vudp_ip_stack_tb___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vudp_ip_stack_tb__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vudp_ip_stack_tb* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vudp_ip_stack_tb___024root     TOP;

    // CONSTRUCTORS
    Vudp_ip_stack_tb__Syms(VerilatedContext* contextp, const char* namep, Vudp_ip_stack_tb* modelp);
    ~Vudp_ip_stack_tb__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
