// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vudp_ip_stack_tb__pch.h"

//============================================================
// Constructors

Vudp_ip_stack_tb::Vudp_ip_stack_tb(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vudp_ip_stack_tb__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vudp_ip_stack_tb::Vudp_ip_stack_tb(const char* _vcname__)
    : Vudp_ip_stack_tb(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vudp_ip_stack_tb::~Vudp_ip_stack_tb() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vudp_ip_stack_tb___024root___eval_debug_assertions(Vudp_ip_stack_tb___024root* vlSelf);
#endif  // VL_DEBUG
void Vudp_ip_stack_tb___024root___eval_static(Vudp_ip_stack_tb___024root* vlSelf);
void Vudp_ip_stack_tb___024root___eval_initial(Vudp_ip_stack_tb___024root* vlSelf);
void Vudp_ip_stack_tb___024root___eval_settle(Vudp_ip_stack_tb___024root* vlSelf);
void Vudp_ip_stack_tb___024root___eval(Vudp_ip_stack_tb___024root* vlSelf);

void Vudp_ip_stack_tb::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vudp_ip_stack_tb::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vudp_ip_stack_tb___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vudp_ip_stack_tb___024root___eval_static(&(vlSymsp->TOP));
        Vudp_ip_stack_tb___024root___eval_initial(&(vlSymsp->TOP));
        Vudp_ip_stack_tb___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vudp_ip_stack_tb___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vudp_ip_stack_tb::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vudp_ip_stack_tb::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vudp_ip_stack_tb::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vudp_ip_stack_tb___024root___eval_final(Vudp_ip_stack_tb___024root* vlSelf);

VL_ATTR_COLD void Vudp_ip_stack_tb::final() {
    Vudp_ip_stack_tb___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vudp_ip_stack_tb::hierName() const { return vlSymsp->name(); }
const char* Vudp_ip_stack_tb::modelName() const { return "Vudp_ip_stack_tb"; }
unsigned Vudp_ip_stack_tb::threads() const { return 1; }
void Vudp_ip_stack_tb::prepareClone() const { contextp()->prepareClone(); }
void Vudp_ip_stack_tb::atClone() const {
    contextp()->threadPoolpOnClone();
}
