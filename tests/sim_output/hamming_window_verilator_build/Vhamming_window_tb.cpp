// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vhamming_window_tb__pch.h"

//============================================================
// Constructors

Vhamming_window_tb::Vhamming_window_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vhamming_window_tb__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vhamming_window_tb::Vhamming_window_tb(const char* _vcname__)
    : Vhamming_window_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vhamming_window_tb::~Vhamming_window_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vhamming_window_tb___024root___eval_debug_assertions(Vhamming_window_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vhamming_window_tb___024root___eval_static(Vhamming_window_tb___024root* vlSelf);
void Vhamming_window_tb___024root___eval_initial(Vhamming_window_tb___024root* vlSelf);
void Vhamming_window_tb___024root___eval_settle(Vhamming_window_tb___024root* vlSelf);
void Vhamming_window_tb___024root___eval(Vhamming_window_tb___024root* vlSelf);

void Vhamming_window_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vhamming_window_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vhamming_window_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vhamming_window_tb___024root___eval_static(&(vlSymsp->TOP));
        Vhamming_window_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vhamming_window_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vhamming_window_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vhamming_window_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vhamming_window_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vhamming_window_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vhamming_window_tb___024root___eval_final(Vhamming_window_tb___024root* vlSelf);

VL_ATTR_COLD void Vhamming_window_tb::final() {
    Vhamming_window_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vhamming_window_tb::hierName() const { return vlSymsp->name(); }
const char* Vhamming_window_tb::modelName() const { return "Vhamming_window_tb"; }
unsigned Vhamming_window_tb::threads() const { return 1; }
void Vhamming_window_tb::prepareClone() const { contextp()->prepareClone(); }
void Vhamming_window_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
