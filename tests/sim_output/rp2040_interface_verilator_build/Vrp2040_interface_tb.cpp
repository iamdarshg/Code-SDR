// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vrp2040_interface_tb__pch.h"

//============================================================
// Constructors

Vrp2040_interface_tb::Vrp2040_interface_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vrp2040_interface_tb__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vrp2040_interface_tb::Vrp2040_interface_tb(const char* _vcname__)
    : Vrp2040_interface_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vrp2040_interface_tb::~Vrp2040_interface_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vrp2040_interface_tb___024root___eval_debug_assertions(Vrp2040_interface_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vrp2040_interface_tb___024root___eval_static(Vrp2040_interface_tb___024root* vlSelf);
void Vrp2040_interface_tb___024root___eval_initial(Vrp2040_interface_tb___024root* vlSelf);
void Vrp2040_interface_tb___024root___eval_settle(Vrp2040_interface_tb___024root* vlSelf);
void Vrp2040_interface_tb___024root___eval(Vrp2040_interface_tb___024root* vlSelf);

void Vrp2040_interface_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vrp2040_interface_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vrp2040_interface_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vrp2040_interface_tb___024root___eval_static(&(vlSymsp->TOP));
        Vrp2040_interface_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vrp2040_interface_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vrp2040_interface_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vrp2040_interface_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vrp2040_interface_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vrp2040_interface_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vrp2040_interface_tb___024root___eval_final(Vrp2040_interface_tb___024root* vlSelf);

VL_ATTR_COLD void Vrp2040_interface_tb::final() {
    Vrp2040_interface_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vrp2040_interface_tb::hierName() const { return vlSymsp->name(); }
const char* Vrp2040_interface_tb::modelName() const { return "Vrp2040_interface_tb"; }
unsigned Vrp2040_interface_tb::threads() const { return 1; }
void Vrp2040_interface_tb::prepareClone() const { contextp()->prepareClone(); }
void Vrp2040_interface_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
