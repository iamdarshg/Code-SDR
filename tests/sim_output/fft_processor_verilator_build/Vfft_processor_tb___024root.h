// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vfft_processor_tb.h for the primary calling header

#ifndef VERILATED_VFFT_PROCESSOR_TB___024ROOT_H_
#define VERILATED_VFFT_PROCESSOR_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vfft_processor_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vfft_processor_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*0:0*/ fft_processor_tb__DOT__clk;
        CData/*0:0*/ fft_processor_tb__DOT__rst_n;
        CData/*0:0*/ fft_processor_tb__DOT__data_valid;
        CData/*0:0*/ fft_processor_tb__DOT__fft_started;
        CData/*7:0*/ fft_processor_tb__DOT__dut__DOT__stage_valid;
        CData/*0:0*/ fft_processor_tb__DOT__dut__DOT__output_valid_reg;
        CData/*7:0*/ __Vdly__fft_processor_tb__DOT__dut__DOT__stage_valid;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__input_real__v0;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__input_real__v0;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__input_imag__v0;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__input_imag__v0;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v0;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v1;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v2;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v3;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v4;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v1;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v5;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v6;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v7;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v3;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v8;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v9;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v10;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v5;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v11;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v12;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v13;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v7;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v14;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v15;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v16;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v9;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10;
    };
    struct {
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v17;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v18;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v19;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v11;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v20;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v21;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v22;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v13;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14;
        CData/*7:0*/ __VdlyDim0__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14;
        CData/*0:0*/ __VdlySet__fft_processor_tb__DOT__dut__DOT__stage_counter__v23;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __Vtrigprevexpr___TOP__fft_processor_tb__DOT__clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__fft_processor_tb__DOT__rst_n__0;
        SData/*11:0*/ fft_processor_tb__DOT__dut__DOT__sample_counter;
        SData/*11:0*/ fft_processor_tb__DOT__dut__DOT__bit_reversed_addr;
        SData/*11:0*/ fft_processor_tb__DOT__dut__DOT__output_counter;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__twiddle_r_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__twiddle_i_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__twiddle_r_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__twiddle_i_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__twiddle_r_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__twiddle_i_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__twiddle_r_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__twiddle_i_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__twiddle_r_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__twiddle_i_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__twiddle_r_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__twiddle_i_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__twiddle_r_val;
        SData/*15:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__twiddle_i_val;
        SData/*11:0*/ __Vfunc_fft_processor_tb__DOT__dut__DOT__bit_reverse__0__Vfuncout;
        SData/*11:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v1;
        SData/*11:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v4;
        SData/*11:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v7;
        SData/*11:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v10;
        SData/*11:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v13;
        SData/*11:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v16;
        SData/*11:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v19;
        SData/*11:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__stage_counter__v22;
        IData/*23:0*/ fft_processor_tb__DOT__real_in;
        IData/*23:0*/ fft_processor_tb__DOT__imag_in;
        IData/*31:0*/ fft_processor_tb__DOT__sample_count;
        IData/*24:0*/ fft_processor_tb__DOT__dut__DOT__saturated_add__Vstatic__sum;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__b_r_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk2__DOT__b_i_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__1__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__b_r_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk2__DOT__b_i_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
    };
    struct {
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__2__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__b_r_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk2__DOT__b_i_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__3__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__b_r_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk2__DOT__b_i_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__4__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__b_r_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk2__DOT__b_i_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__5__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__b_r_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk2__DOT__b_i_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__6__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__b_r_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk2__DOT__b_i_scaled;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk3__DOT__bw_real_rounded;
        IData/*23:0*/ fft_processor_tb__DOT__dut__DOT__stage_control__BRA__7__KET____DOT__unnamedblk3__DOT__bw_imag_rounded;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__input_real__v0;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__input_imag__v0;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v0;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v0;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v1;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v1;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v2;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v2;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v3;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v3;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v4;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v4;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v5;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v5;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v6;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v6;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v7;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v7;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v8;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v8;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v9;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v9;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v10;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v10;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v11;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v11;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v12;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v12;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v13;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v13;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_real__v14;
        IData/*23:0*/ __VdlyVal__fft_processor_tb__DOT__dut__DOT__fft_buffer_imag__v14;
        IData/*31:0*/ __VactIterCount;
        VlUnpacked<IData/*23:0*/, 256> fft_processor_tb__DOT__dut__DOT__input_real;
        VlUnpacked<IData/*23:0*/, 256> fft_processor_tb__DOT__dut__DOT__input_imag;
        VlUnpacked<SData/*15:0*/, 128> fft_processor_tb__DOT__dut__DOT__twiddle_real;
        VlUnpacked<SData/*15:0*/, 128> fft_processor_tb__DOT__dut__DOT__twiddle_imag;
        VlUnpacked<VlUnpacked<IData/*23:0*/, 256>, 2> fft_processor_tb__DOT__dut__DOT__fft_buffer_real;
        VlUnpacked<VlUnpacked<IData/*23:0*/, 256>, 2> fft_processor_tb__DOT__dut__DOT__fft_buffer_imag;
        VlUnpacked<SData/*11:0*/, 8> fft_processor_tb__DOT__dut__DOT__stage_counter;
        VlUnpacked<CData/*7:0*/, 8> fft_processor_tb__DOT__dut__DOT__butterfly_size_reg;
        VlUnpacked<SData/*11:0*/, 8> fft_processor_tb__DOT__dut__DOT__butterfly_index_reg;
        VlUnpacked<SData/*11:0*/, 8> fft_processor_tb__DOT__dut__DOT__group_index_reg;
    };
    struct {
        VlUnpacked<SData/*11:0*/, 8> fft_processor_tb__DOT__dut__DOT__upper_index_reg;
        VlUnpacked<SData/*10:0*/, 8> fft_processor_tb__DOT__dut__DOT__twiddle_index_reg;
        VlUnpacked<IData/*23:0*/, 8> fft_processor_tb__DOT__dut__DOT__a_r_reg;
        VlUnpacked<IData/*23:0*/, 8> fft_processor_tb__DOT__dut__DOT__a_i_reg;
        VlUnpacked<IData/*23:0*/, 8> fft_processor_tb__DOT__dut__DOT__b_r_reg;
        VlUnpacked<IData/*23:0*/, 8> fft_processor_tb__DOT__dut__DOT__b_i_reg;
        VlUnpacked<QData/*47:0*/, 8> fft_processor_tb__DOT__dut__DOT__mult_result_real_reg;
        VlUnpacked<QData/*47:0*/, 8> fft_processor_tb__DOT__dut__DOT__mult_result_imag_reg;
        VlUnpacked<IData/*23:0*/, 8> fft_processor_tb__DOT__dut__DOT__output_real_top_reg;
        VlUnpacked<IData/*23:0*/, 8> fft_processor_tb__DOT__dut__DOT__output_imag_top_reg;
        VlUnpacked<IData/*23:0*/, 8> fft_processor_tb__DOT__dut__DOT__output_real_bottom_reg;
        VlUnpacked<IData/*23:0*/, 8> fft_processor_tb__DOT__dut__DOT__output_imag_bottom_reg;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };
    double fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__angle;
    double fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__cos_val;
    double fft_processor_tb__DOT__dut__DOT__unnamedblk1__DOT__sin_val;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h7903b55b__0;

    // INTERNAL VARIABLES
    Vfft_processor_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vfft_processor_tb___024root(Vfft_processor_tb__Syms* symsp, const char* v__name);
    ~Vfft_processor_tb___024root();
    VL_UNCOPYABLE(Vfft_processor_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
