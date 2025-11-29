// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vaverage_power_detector_tb__pch.h"

//============================================================
// Constructors

Vaverage_power_detector_tb::Vaverage_power_detector_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vaverage_power_detector_tb__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vaverage_power_detector_tb::Vaverage_power_detector_tb(const char* _vcname__)
    : Vaverage_power_detector_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vaverage_power_detector_tb::~Vaverage_power_detector_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vaverage_power_detector_tb___024root___eval_debug_assertions(Vaverage_power_detector_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vaverage_power_detector_tb___024root___eval_static(Vaverage_power_detector_tb___024root* vlSelf);
void Vaverage_power_detector_tb___024root___eval_initial(Vaverage_power_detector_tb___024root* vlSelf);
void Vaverage_power_detector_tb___024root___eval_settle(Vaverage_power_detector_tb___024root* vlSelf);
void Vaverage_power_detector_tb___024root___eval(Vaverage_power_detector_tb___024root* vlSelf);

void Vaverage_power_detector_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vaverage_power_detector_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vaverage_power_detector_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vaverage_power_detector_tb___024root___eval_static(&(vlSymsp->TOP));
        Vaverage_power_detector_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vaverage_power_detector_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vaverage_power_detector_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vaverage_power_detector_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vaverage_power_detector_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vaverage_power_detector_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vaverage_power_detector_tb___024root___eval_final(Vaverage_power_detector_tb___024root* vlSelf);

VL_ATTR_COLD void Vaverage_power_detector_tb::final() {
    Vaverage_power_detector_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vaverage_power_detector_tb::hierName() const { return vlSymsp->name(); }
const char* Vaverage_power_detector_tb::modelName() const { return "Vaverage_power_detector_tb"; }
unsigned Vaverage_power_detector_tb::threads() const { return 1; }
void Vaverage_power_detector_tb::prepareClone() const { contextp()->prepareClone(); }
void Vaverage_power_detector_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
