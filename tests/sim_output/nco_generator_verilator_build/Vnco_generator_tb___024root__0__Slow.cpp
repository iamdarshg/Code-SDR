// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vnco_generator_tb.h for the primary calling header

#include "Vnco_generator_tb__pch.h"

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_static(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_static\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__nco_generator_tb__DOT__clk__0 
        = vlSelfRef.nco_generator_tb__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__nco_generator_tb__DOT__rst_n__0 
        = vlSelfRef.nco_generator_tb__DOT__rst_n;
}

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_initial__TOP(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_initial__TOP\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0U] = 0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0U] = 0x7fffU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[1U] = 0x00c9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[1U] = 0x7ffeU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[2U] = 0x0192U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[2U] = 0x7ffdU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[3U] = 0x025cU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[3U] = 0x7ffbU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[4U] = 0x0325U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[4U] = 0x7ff8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[5U] = 0x03eeU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[5U] = 0x7ff4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[6U] = 0x04b8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[6U] = 0x7ff0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[7U] = 0x0581U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[7U] = 0x7febU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[8U] = 0x064aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[8U] = 0x7fe5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[9U] = 0x0713U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[9U] = 0x7fdeU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x0aU] = 0x07dcU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x0aU] = 0x7fd7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x0bU] = 0x08a5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x0bU] = 0x7fcfU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x0cU] = 0x096eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x0cU] = 0x7fc6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x0dU] = 0x0a37U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x0dU] = 0x7fbcU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x0eU] = 0x0b00U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x0eU] = 0x7fb1U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x0fU] = 0x0bc9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x0fU] = 0x7fa5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x10U] = 0x0c91U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x10U] = 0x7f99U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x11U] = 0x0d59U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x11U] = 0x7f8bU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x12U] = 0x0e21U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x12U] = 0x7f7cU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x13U] = 0x0ee9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x13U] = 0x7f6cU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x14U] = 0x0fb1U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x14U] = 0x7f5bU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x15U] = 0x1079U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x15U] = 0x7f49U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x16U] = 0x1140U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x16U] = 0x7f36U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x17U] = 0x1207U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x17U] = 0x7f22U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x18U] = 0x12cdU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x18U] = 0x7f0dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x19U] = 0x1393U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x19U] = 0x7ef7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x1aU] = 0x1458U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x1aU] = 0x7edfU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x1bU] = 0x151dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x1bU] = 0x7ec7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x1cU] = 0x15e1U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x1cU] = 0x7eadU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x1dU] = 0x16a4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x1dU] = 0x7e92U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x1eU] = 0x1767U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x1eU] = 0x7e76U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x1fU] = 0x1829U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x1fU] = 0x7e59U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x20U] = 0x18eaU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x20U] = 0x7e3bU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x21U] = 0x19abU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x21U] = 0x7e1cU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x22U] = 0x1a6aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x22U] = 0x7dfbU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x23U] = 0x1b28U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x23U] = 0x7dd9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x24U] = 0x1be6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x24U] = 0x7db6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x25U] = 0x1ca2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x25U] = 0x7d92U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x26U] = 0x1d5eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x26U] = 0x7d6dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x27U] = 0x1e18U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x27U] = 0x7d46U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x28U] = 0x1ed2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x28U] = 0x7d1eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x29U] = 0x1f8aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x29U] = 0x7cf5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x2aU] = 0x2041U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x2aU] = 0x7ccbU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x2bU] = 0x20f7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x2bU] = 0x7c9fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x2cU] = 0x21abU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x2cU] = 0x7c72U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x2dU] = 0x225fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x2dU] = 0x7c44U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x2eU] = 0x2311U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x2eU] = 0x7c15U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x2fU] = 0x23c2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x2fU] = 0x7be4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x30U] = 0x2472U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x30U] = 0x7bb2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x31U] = 0x2520U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x31U] = 0x7b7fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x32U] = 0x25cdU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x32U] = 0x7b4bU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x33U] = 0x2679U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x33U] = 0x7b15U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x34U] = 0x2723U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x34U] = 0x7adfU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x35U] = 0x27ccU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x35U] = 0x7aa6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x36U] = 0x2873U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x36U] = 0x7a6dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x37U] = 0x2919U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x37U] = 0x7a32U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x38U] = 0x29bdU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x38U] = 0x79f6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x39U] = 0x2a60U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x39U] = 0x79b8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x3aU] = 0x2b01U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x3aU] = 0x797aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x3bU] = 0x2ba0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x3bU] = 0x793aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x3cU] = 0x2c3eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x3cU] = 0x78f9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x3dU] = 0x2cdaU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x3dU] = 0x78b7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x3eU] = 0x2d74U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x3eU] = 0x7874U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x3fU] = 0x2e0dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x3fU] = 0x782fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x40U] = 0x2ea4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x40U] = 0x77e9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x41U] = 0x2f39U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x41U] = 0x77a2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x42U] = 0x2fccU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x42U] = 0x775aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x43U] = 0x305eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x43U] = 0x7710U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x44U] = 0x30eeU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x44U] = 0x76c5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x45U] = 0x317cU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x45U] = 0x7678U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x46U] = 0x3208U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x46U] = 0x762aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x47U] = 0x3292U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x47U] = 0x75dbU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x48U] = 0x331aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x48U] = 0x758aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x49U] = 0x33a1U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x49U] = 0x7538U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x4aU] = 0x3425U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x4aU] = 0x74e4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x4bU] = 0x34a8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x4bU] = 0x748fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x4cU] = 0x3528U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x4cU] = 0x7439U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x4dU] = 0x35a7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x4dU] = 0x73e1U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x4eU] = 0x3624U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x4eU] = 0x7388U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x4fU] = 0x369eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x4fU] = 0x732eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x50U] = 0x3717U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x50U] = 0x72d2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x51U] = 0x378dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x51U] = 0x7275U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x52U] = 0x3801U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x52U] = 0x7216U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x53U] = 0x3873U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x53U] = 0x71b6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x54U] = 0x38e3U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x54U] = 0x7155U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x55U] = 0x3951U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x55U] = 0x70f2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x56U] = 0x39bcU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x56U] = 0x708eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x57U] = 0x3a26U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x57U] = 0x7028U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x58U] = 0x3a8dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x58U] = 0x6fc1U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x59U] = 0x3af2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x59U] = 0x6f59U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x5aU] = 0x3b55U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x5aU] = 0x6eefU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x5bU] = 0x3bb6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x5bU] = 0x6e84U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x5cU] = 0x3c14U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x5cU] = 0x6e17U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x5dU] = 0x3c70U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x5dU] = 0x6da9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x5eU] = 0x3ccaU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x5eU] = 0x6d3aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x5fU] = 0x3d22U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x5fU] = 0x6cc9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x60U] = 0x3d77U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x60U] = 0x6c57U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x61U] = 0x3dcaU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x61U] = 0x6be4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x62U] = 0x3e1bU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x62U] = 0x6b6fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x63U] = 0x3e6aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x63U] = 0x6af9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x64U] = 0x3eb6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x64U] = 0x6a81U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x65U] = 0x3f00U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x65U] = 0x6a08U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x66U] = 0x3f48U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x66U] = 0x698eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x67U] = 0x3f8dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x67U] = 0x6912U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x68U] = 0x3fd0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x68U] = 0x6895U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x69U] = 0x4011U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x69U] = 0x6817U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x6aU] = 0x404fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x6aU] = 0x6797U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x6bU] = 0x408bU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x6bU] = 0x6716U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x6cU] = 0x40c4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x6cU] = 0x6693U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x6dU] = 0x40fbU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x6dU] = 0x660fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x6eU] = 0x4130U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x6eU] = 0x6589U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x6fU] = 0x4162U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x6fU] = 0x6502U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x70U] = 0x4191U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x70U] = 0x6479U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x71U] = 0x41beU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x71U] = 0x63efU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x72U] = 0x41e8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x72U] = 0x6364U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x73U] = 0x4210U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x73U] = 0x62d6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x74U] = 0x4235U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x74U] = 0x6248U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x75U] = 0x4257U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x75U] = 0x61b8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x76U] = 0x4277U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x76U] = 0x6126U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x77U] = 0x4294U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x77U] = 0x6093U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x78U] = 0x42aeU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x78U] = 0x5fffU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x79U] = 0x42c5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x79U] = 0x5f69U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x7aU] = 0x42daU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x7aU] = 0x5ed2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x7bU] = 0x42ecU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x7bU] = 0x5e39U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x7cU] = 0x42fbU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x7cU] = 0x5d9fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x7dU] = 0x4307U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x7dU] = 0x5d04U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x7eU] = 0x4311U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x7eU] = 0x5c67U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x7fU] = 0x4317U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x7fU] = 0x5bc9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x80U] = 0x4319U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x80U] = 0x5b2aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x81U] = 0x4317U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x81U] = 0x5a8aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x82U] = 0x4311U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x82U] = 0x59e8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x83U] = 0x4307U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x83U] = 0x5946U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x84U] = 0x42fbU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x84U] = 0x58a2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x85U] = 0x42ecU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x85U] = 0x57fdU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x86U] = 0x42daU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x86U] = 0x5757U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x87U] = 0x42c5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x87U] = 0x56afU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x88U] = 0x42aeU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x88U] = 0x5606U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x89U] = 0x4294U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x89U] = 0x555cU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x8aU] = 0x4277U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x8aU] = 0x54b0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x8bU] = 0x4257U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x8bU] = 0x5404U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x8cU] = 0x4235U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x8cU] = 0x5356U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x8dU] = 0x4210U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x8dU] = 0x52a7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x8eU] = 0x41e8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x8eU] = 0x51f6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x8fU] = 0x41beU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x8fU] = 0x5145U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x90U] = 0x4191U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x90U] = 0x5092U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x91U] = 0x4162U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x91U] = 0x4fdeU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x92U] = 0x4130U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x92U] = 0x4f29U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x93U] = 0x40fbU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x93U] = 0x4e73U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x94U] = 0x40c4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x94U] = 0x4dbcU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x95U] = 0x408bU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x95U] = 0x4d03U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x96U] = 0x404fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x96U] = 0x4c49U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x97U] = 0x4011U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x97U] = 0x4b8eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x98U] = 0x3fd0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x98U] = 0x4ad2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x99U] = 0x3f8dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x99U] = 0x4a15U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x9aU] = 0x3f48U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x9aU] = 0x4956U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x9bU] = 0x3f00U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x9bU] = 0x4896U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x9cU] = 0x3eb6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x9cU] = 0x47d5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x9dU] = 0x3e6aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x9dU] = 0x4713U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x9eU] = 0x3e1bU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x9eU] = 0x4650U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0x9fU] = 0x3dcaU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0x9fU] = 0x458bU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xa0U] = 0x3d77U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xa0U] = 0x44c6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xa1U] = 0x3d22U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xa1U] = 0x43ffU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xa2U] = 0x3ccaU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xa2U] = 0x4337U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xa3U] = 0x3c70U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xa3U] = 0x426fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xa4U] = 0x3c14U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xa4U] = 0x41a5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xa5U] = 0x3bb6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xa5U] = 0x40daU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xa6U] = 0x3b55U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xa6U] = 0x400eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xa7U] = 0x3af2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xa7U] = 0x3f41U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xa8U] = 0x3a8dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xa8U] = 0x3e73U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xa9U] = 0x3a26U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xa9U] = 0x3da4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xaaU] = 0x39bcU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xaaU] = 0x3cd4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xabU] = 0x3951U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xabU] = 0x3c03U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xacU] = 0x38e3U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xacU] = 0x3b30U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xadU] = 0x3873U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xadU] = 0x3a5cU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xaeU] = 0x3801U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xaeU] = 0x3988U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xafU] = 0x378dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xafU] = 0x38b2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xb0U] = 0x3717U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xb0U] = 0x37dbU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xb1U] = 0x369eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xb1U] = 0x3703U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xb2U] = 0x3624U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xb2U] = 0x362aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xb3U] = 0x35a7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xb3U] = 0x354fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xb4U] = 0x3528U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xb4U] = 0x3474U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xb5U] = 0x34a8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xb5U] = 0x3397U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xb6U] = 0x3425U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xb6U] = 0x32b9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xb7U] = 0x33a1U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xb7U] = 0x31d9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xb8U] = 0x331aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xb8U] = 0x30f8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xb9U] = 0x3292U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xb9U] = 0x3016U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xbaU] = 0x3208U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xbaU] = 0x2f32U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xbbU] = 0x317cU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xbbU] = 0x2e4dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xbcU] = 0x30eeU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xbcU] = 0x2d67U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xbdU] = 0x305eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xbdU] = 0x2c80U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xbeU] = 0x2fccU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xbeU] = 0x2b97U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xbfU] = 0x2f39U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xbfU] = 0x2aadU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xc0U] = 0x2ea4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xc0U] = 0x29c2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xc1U] = 0x2e0dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xc1U] = 0x28d5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xc2U] = 0x2d74U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xc2U] = 0x27e7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xc3U] = 0x2cdaU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xc3U] = 0x26f8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xc4U] = 0x2c3eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xc4U] = 0x2607U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xc5U] = 0x2ba0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xc5U] = 0x2516U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xc6U] = 0x2b01U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xc6U] = 0x2423U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xc7U] = 0x2a60U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xc7U] = 0x232fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xc8U] = 0x29bdU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xc8U] = 0x223aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xc9U] = 0x2919U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xc9U] = 0x2144U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xcaU] = 0x2873U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xcaU] = 0x204dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xcbU] = 0x27ccU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xcbU] = 0x1f54U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xccU] = 0x2723U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xccU] = 0x1e5aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xcdU] = 0x2679U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xcdU] = 0x1d5eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xceU] = 0x25cdU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xceU] = 0x1c62U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xcfU] = 0x2520U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xcfU] = 0x1b64U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xd0U] = 0x2472U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xd0U] = 0x1a65U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xd1U] = 0x23c2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xd1U] = 0x1965U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xd2U] = 0x2311U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xd2U] = 0x1864U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xd3U] = 0x225fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xd3U] = 0x1762U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xd4U] = 0x21abU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xd4U] = 0x165fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xd5U] = 0x20f7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xd5U] = 0x155bU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xd6U] = 0x2041U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xd6U] = 0x1456U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xd7U] = 0x1f8aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xd7U] = 0x1350U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xd8U] = 0x1ed2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xd8U] = 0x1249U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xd9U] = 0x1e18U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xd9U] = 0x1141U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xdaU] = 0x1d5eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xdaU] = 0x1039U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xdbU] = 0x1ca2U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xdbU] = 0x0f2fU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xdcU] = 0x1be6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xdcU] = 0x0e24U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xddU] = 0x1b28U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xddU] = 0x0d19U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xdeU] = 0x1a6aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xdeU] = 0x0c0dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xdfU] = 0x19abU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xdfU] = 0x0b01U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xe0U] = 0x18eaU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xe0U] = 0x09f3U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xe1U] = 0x1829U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xe1U] = 0x08e5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xe2U] = 0x1767U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xe2U] = 0x07d6U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xe3U] = 0x16a4U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xe3U] = 0x06c7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xe4U] = 0x15e1U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xe4U] = 0x05b7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xe5U] = 0x151dU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xe5U] = 0x04a7U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xe6U] = 0x1458U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xe6U] = 0x0397U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xe7U] = 0x1393U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xe7U] = 0x0286U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xe8U] = 0x12cdU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xe8U] = 0x0175U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xe9U] = 0x1207U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xe9U] = 0x0064U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xeaU] = 0x1140U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xeaU] = 0xff54U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xebU] = 0x1079U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xebU] = 0xfe43U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xecU] = 0x0fb1U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xecU] = 0xfd32U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xedU] = 0x0ee9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xedU] = 0xfc21U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xeeU] = 0x0e21U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xeeU] = 0xfb00U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xefU] = 0x0d59U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xefU] = 0xf9f0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xf0U] = 0x0c91U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xf0U] = 0xf8f0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xf1U] = 0x0bc9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xf1U] = 0xf7e0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xf2U] = 0x0b00U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xf2U] = 0xf6d0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xf3U] = 0x0a37U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xf3U] = 0xf5c0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xf4U] = 0x096eU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xf4U] = 0xf4c0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xf5U] = 0x08a5U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xf5U] = 0xf3b0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xf6U] = 0x07dcU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xf6U] = 0xf2a0U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xf7U] = 0x0713U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xf7U] = 0xf190U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xf8U] = 0x064aU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xf8U] = 0xf080U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xf9U] = 0x0581U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xf9U] = 0xef70U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xfaU] = 0x04b8U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xfaU] = 0xee60U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xfbU] = 0x03eeU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xfbU] = 0xed50U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xfcU] = 0x0325U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xfcU] = 0xec40U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xfdU] = 0x025cU;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xfdU] = 0xeb30U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xfeU] = 0x0192U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xfeU] = 0xea20U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__sin_rom[0xffU] = 0x00c9U;
    vlSelfRef.nco_generator_tb__DOT__dut__DOT__cos_rom[0xffU] = 0xe910U;
}

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_final(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_final\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vnco_generator_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vnco_generator_tb___024root___eval_phase__stl(Vnco_generator_tb___024root* vlSelf);

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_settle(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_settle\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vnco_generator_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("verilog/nco_generator_tb.v", 3, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vnco_generator_tb___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_triggers__stl(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_triggers__stl\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vnco_generator_tb___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vnco_generator_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vnco_generator_tb___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vnco_generator_tb___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vnco_generator_tb___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___trigger_anySet__stl\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vnco_generator_tb___024root___act_sequent__TOP__0(Vnco_generator_tb___024root* vlSelf);

VL_ATTR_COLD void Vnco_generator_tb___024root___eval_stl(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_stl\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vnco_generator_tb___024root___act_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vnco_generator_tb___024root___eval_phase__stl(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___eval_phase__stl\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vnco_generator_tb___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vnco_generator_tb___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vnco_generator_tb___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vnco_generator_tb___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vnco_generator_tb___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vnco_generator_tb___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge nco_generator_tb.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge nco_generator_tb.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vnco_generator_tb___024root___ctor_var_reset(Vnco_generator_tb___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vnco_generator_tb___024root___ctor_var_reset\n"); );
    Vnco_generator_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->nco_generator_tb__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7013447719023367527ull);
    vlSelf->nco_generator_tb__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17040999325873965629ull);
    vlSelf->nco_generator_tb__DOT__frequency_word = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12459102510333918098ull);
    vlSelf->nco_generator_tb__DOT__enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4713346959820286104ull);
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->nco_generator_tb__DOT__dut__DOT__sin_rom[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7607983075232551432ull);
    }
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->nco_generator_tb__DOT__dut__DOT__cos_rom[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1255098502210238550ull);
    }
    vlSelf->nco_generator_tb__DOT__dut__DOT__phase_accumulator = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 16347092544274596520ull);
    vlSelf->nco_generator_tb__DOT__dut__DOT__phase_accumulator_next = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12379754452741059737ull);
    vlSelf->nco_generator_tb__DOT__dut__DOT__sine_reg = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16849890523114976822ull);
    vlSelf->nco_generator_tb__DOT__dut__DOT__cosine_reg = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8854082886331771150ull);
    vlSelf->nco_generator_tb__DOT__dut__DOT__valid_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10247284853561029685ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__nco_generator_tb__DOT__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11150424196796654784ull);
    vlSelf->__Vtrigprevexpr___TOP__nco_generator_tb__DOT__rst_n__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13158991576343744825ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
