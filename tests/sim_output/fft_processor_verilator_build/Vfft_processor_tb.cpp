// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vfft_processor_tb__pch.h"

//============================================================
// Constructors

Vfft_processor_tb::Vfft_processor_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vfft_processor_tb__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vfft_processor_tb::Vfft_processor_tb(const char* _vcname__)
    : Vfft_processor_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vfft_processor_tb::~Vfft_processor_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vfft_processor_tb___024root___eval_debug_assertions(Vfft_processor_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vfft_processor_tb___024root___eval_static(Vfft_processor_tb___024root* vlSelf);
void Vfft_processor_tb___024root___eval_initial(Vfft_processor_tb___024root* vlSelf);
void Vfft_processor_tb___024root___eval_settle(Vfft_processor_tb___024root* vlSelf);
void Vfft_processor_tb___024root___eval(Vfft_processor_tb___024root* vlSelf);

void Vfft_processor_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vfft_processor_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vfft_processor_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vfft_processor_tb___024root___eval_static(&(vlSymsp->TOP));
        Vfft_processor_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vfft_processor_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vfft_processor_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vfft_processor_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vfft_processor_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vfft_processor_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vfft_processor_tb___024root___eval_final(Vfft_processor_tb___024root* vlSelf);

VL_ATTR_COLD void Vfft_processor_tb::final() {
    Vfft_processor_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vfft_processor_tb::hierName() const { return vlSymsp->name(); }
const char* Vfft_processor_tb::modelName() const { return "Vfft_processor_tb"; }
unsigned Vfft_processor_tb::threads() const { return 1; }
void Vfft_processor_tb::prepareClone() const { contextp()->prepareClone(); }
void Vfft_processor_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
