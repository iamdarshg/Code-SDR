// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vnco_generator_tb__pch.h"

//============================================================
// Constructors

Vnco_generator_tb::Vnco_generator_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vnco_generator_tb__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vnco_generator_tb::Vnco_generator_tb(const char* _vcname__)
    : Vnco_generator_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vnco_generator_tb::~Vnco_generator_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vnco_generator_tb___024root___eval_debug_assertions(Vnco_generator_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vnco_generator_tb___024root___eval_static(Vnco_generator_tb___024root* vlSelf);
void Vnco_generator_tb___024root___eval_initial(Vnco_generator_tb___024root* vlSelf);
void Vnco_generator_tb___024root___eval_settle(Vnco_generator_tb___024root* vlSelf);
void Vnco_generator_tb___024root___eval(Vnco_generator_tb___024root* vlSelf);

void Vnco_generator_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vnco_generator_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vnco_generator_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vnco_generator_tb___024root___eval_static(&(vlSymsp->TOP));
        Vnco_generator_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vnco_generator_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vnco_generator_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vnco_generator_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vnco_generator_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vnco_generator_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vnco_generator_tb___024root___eval_final(Vnco_generator_tb___024root* vlSelf);

VL_ATTR_COLD void Vnco_generator_tb::final() {
    Vnco_generator_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vnco_generator_tb::hierName() const { return vlSymsp->name(); }
const char* Vnco_generator_tb::modelName() const { return "Vnco_generator_tb"; }
unsigned Vnco_generator_tb::threads() const { return 1; }
void Vnco_generator_tb::prepareClone() const { contextp()->prepareClone(); }
void Vnco_generator_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
