// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vadc_interface_tb__pch.h"

//============================================================
// Constructors

Vadc_interface_tb::Vadc_interface_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vadc_interface_tb__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vadc_interface_tb::Vadc_interface_tb(const char* _vcname__)
    : Vadc_interface_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vadc_interface_tb::~Vadc_interface_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vadc_interface_tb___024root___eval_debug_assertions(Vadc_interface_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vadc_interface_tb___024root___eval_static(Vadc_interface_tb___024root* vlSelf);
void Vadc_interface_tb___024root___eval_initial(Vadc_interface_tb___024root* vlSelf);
void Vadc_interface_tb___024root___eval_settle(Vadc_interface_tb___024root* vlSelf);
void Vadc_interface_tb___024root___eval(Vadc_interface_tb___024root* vlSelf);

void Vadc_interface_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vadc_interface_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vadc_interface_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vadc_interface_tb___024root___eval_static(&(vlSymsp->TOP));
        Vadc_interface_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vadc_interface_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vadc_interface_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vadc_interface_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vadc_interface_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vadc_interface_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vadc_interface_tb___024root___eval_final(Vadc_interface_tb___024root* vlSelf);

VL_ATTR_COLD void Vadc_interface_tb::final() {
    Vadc_interface_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vadc_interface_tb::hierName() const { return vlSymsp->name(); }
const char* Vadc_interface_tb::modelName() const { return "Vadc_interface_tb"; }
unsigned Vadc_interface_tb::threads() const { return 1; }
void Vadc_interface_tb::prepareClone() const { contextp()->prepareClone(); }
void Vadc_interface_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
