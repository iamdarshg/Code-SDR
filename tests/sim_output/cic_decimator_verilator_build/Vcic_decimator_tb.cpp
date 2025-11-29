// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vcic_decimator_tb__pch.h"

//============================================================
// Constructors

Vcic_decimator_tb::Vcic_decimator_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vcic_decimator_tb__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vcic_decimator_tb::Vcic_decimator_tb(const char* _vcname__)
    : Vcic_decimator_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vcic_decimator_tb::~Vcic_decimator_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vcic_decimator_tb___024root___eval_debug_assertions(Vcic_decimator_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vcic_decimator_tb___024root___eval_static(Vcic_decimator_tb___024root* vlSelf);
void Vcic_decimator_tb___024root___eval_initial(Vcic_decimator_tb___024root* vlSelf);
void Vcic_decimator_tb___024root___eval_settle(Vcic_decimator_tb___024root* vlSelf);
void Vcic_decimator_tb___024root___eval(Vcic_decimator_tb___024root* vlSelf);

void Vcic_decimator_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vcic_decimator_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vcic_decimator_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vcic_decimator_tb___024root___eval_static(&(vlSymsp->TOP));
        Vcic_decimator_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vcic_decimator_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vcic_decimator_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vcic_decimator_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vcic_decimator_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vcic_decimator_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vcic_decimator_tb___024root___eval_final(Vcic_decimator_tb___024root* vlSelf);

VL_ATTR_COLD void Vcic_decimator_tb::final() {
    Vcic_decimator_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vcic_decimator_tb::hierName() const { return vlSymsp->name(); }
const char* Vcic_decimator_tb::modelName() const { return "Vcic_decimator_tb"; }
unsigned Vcic_decimator_tb::threads() const { return 1; }
void Vcic_decimator_tb::prepareClone() const { contextp()->prepareClone(); }
void Vcic_decimator_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
