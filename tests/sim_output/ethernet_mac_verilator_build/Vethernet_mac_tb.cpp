// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vethernet_mac_tb__pch.h"

//============================================================
// Constructors

Vethernet_mac_tb::Vethernet_mac_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vethernet_mac_tb__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vethernet_mac_tb::Vethernet_mac_tb(const char* _vcname__)
    : Vethernet_mac_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vethernet_mac_tb::~Vethernet_mac_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vethernet_mac_tb___024root___eval_debug_assertions(Vethernet_mac_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vethernet_mac_tb___024root___eval_static(Vethernet_mac_tb___024root* vlSelf);
void Vethernet_mac_tb___024root___eval_initial(Vethernet_mac_tb___024root* vlSelf);
void Vethernet_mac_tb___024root___eval_settle(Vethernet_mac_tb___024root* vlSelf);
void Vethernet_mac_tb___024root___eval(Vethernet_mac_tb___024root* vlSelf);

void Vethernet_mac_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vethernet_mac_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vethernet_mac_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vethernet_mac_tb___024root___eval_static(&(vlSymsp->TOP));
        Vethernet_mac_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vethernet_mac_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vethernet_mac_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vethernet_mac_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vethernet_mac_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vethernet_mac_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vethernet_mac_tb___024root___eval_final(Vethernet_mac_tb___024root* vlSelf);

VL_ATTR_COLD void Vethernet_mac_tb::final() {
    Vethernet_mac_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vethernet_mac_tb::hierName() const { return vlSymsp->name(); }
const char* Vethernet_mac_tb::modelName() const { return "Vethernet_mac_tb"; }
unsigned Vethernet_mac_tb::threads() const { return 1; }
void Vethernet_mac_tb::prepareClone() const { contextp()->prepareClone(); }
void Vethernet_mac_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
