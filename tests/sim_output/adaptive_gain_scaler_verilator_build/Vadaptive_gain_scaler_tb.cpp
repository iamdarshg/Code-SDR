// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vadaptive_gain_scaler_tb__pch.h"

//============================================================
// Constructors

Vadaptive_gain_scaler_tb::Vadaptive_gain_scaler_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vadaptive_gain_scaler_tb__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vadaptive_gain_scaler_tb::Vadaptive_gain_scaler_tb(const char* _vcname__)
    : Vadaptive_gain_scaler_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vadaptive_gain_scaler_tb::~Vadaptive_gain_scaler_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vadaptive_gain_scaler_tb___024root___eval_debug_assertions(Vadaptive_gain_scaler_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vadaptive_gain_scaler_tb___024root___eval_static(Vadaptive_gain_scaler_tb___024root* vlSelf);
void Vadaptive_gain_scaler_tb___024root___eval_initial(Vadaptive_gain_scaler_tb___024root* vlSelf);
void Vadaptive_gain_scaler_tb___024root___eval_settle(Vadaptive_gain_scaler_tb___024root* vlSelf);
void Vadaptive_gain_scaler_tb___024root___eval(Vadaptive_gain_scaler_tb___024root* vlSelf);

void Vadaptive_gain_scaler_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vadaptive_gain_scaler_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vadaptive_gain_scaler_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vadaptive_gain_scaler_tb___024root___eval_static(&(vlSymsp->TOP));
        Vadaptive_gain_scaler_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vadaptive_gain_scaler_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vadaptive_gain_scaler_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vadaptive_gain_scaler_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vadaptive_gain_scaler_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vadaptive_gain_scaler_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vadaptive_gain_scaler_tb___024root___eval_final(Vadaptive_gain_scaler_tb___024root* vlSelf);

VL_ATTR_COLD void Vadaptive_gain_scaler_tb::final() {
    Vadaptive_gain_scaler_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vadaptive_gain_scaler_tb::hierName() const { return vlSymsp->name(); }
const char* Vadaptive_gain_scaler_tb::modelName() const { return "Vadaptive_gain_scaler_tb"; }
unsigned Vadaptive_gain_scaler_tb::threads() const { return 1; }
void Vadaptive_gain_scaler_tb::prepareClone() const { contextp()->prepareClone(); }
void Vadaptive_gain_scaler_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
