// ============================================================================
// NCO Generator Module
// ============================================================================
// Numerically Controlled Oscillator for digital downconversion
// ============================================================================

`timescale 1ns/1ps

module nco_generator #(
    parameter WIDTH = 32,
    parameter ROM_DEPTH = 256
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [31:0] frequency_word,  // NCO frequency word
    input  wire        enable,          // NCO enable
    output wire [15:0] sine_out,        // Sine output (16-bit)
    output wire [15:0] cosine_out,      // Cosine output (16-bit)
    output wire        valid_out        // Output valid flag
);

    // ========================================================================
    // ROM table for sine/cosine lookup
    // ========================================================================

    // Initialize ROM with sine and cosine values
    reg [15:0] sin_rom [0:ROM_DEPTH-1];
    reg [15:0] cos_rom [0:ROM_DEPTH-1];
    integer rom_init_i;

    // ROM initialization block (pre-calculated for Verilator compatibility)
    initial begin
        // These values are pre-calculated using:
        // sin_rom[i] = $rtoi($sin((i * 3.14159265358979323846) / (2.0 * ROM_DEPTH)) * 32767.0);
        // cos_rom[i] = $rtoi($cos((i * 3.14159265358979323846) / (2.0 * ROM_DEPTH)) * 32767.0);
        // for ROM_DEPTH = 256
        sin_rom[0] = 16'd0;
        cos_rom[0] = 16'd32767;
        sin_rom[1] = 16'd201;
        cos_rom[1] = 16'd32766;
        sin_rom[2] = 16'd402;
        cos_rom[2] = 16'd32765;
        sin_rom[3] = 16'd604;
        cos_rom[3] = 16'd32763;
        sin_rom[4] = 16'd805;
        cos_rom[4] = 16'd32760;
        sin_rom[5] = 16'd1006;
        cos_rom[5] = 16'd32756;
        sin_rom[6] = 16'd1208;
        cos_rom[6] = 16'd32752;
        sin_rom[7] = 16'd1409;
        cos_rom[7] = 16'd32747;
        sin_rom[8] = 16'd1610;
        cos_rom[8] = 16'd32741;
        sin_rom[9] = 16'd1811;
        cos_rom[9] = 16'd32734;
        sin_rom[10] = 16'd2012;
        cos_rom[10] = 16'd32727;
        sin_rom[11] = 16'd2213;
        cos_rom[11] = 16'd32719;
        sin_rom[12] = 16'd2414;
        cos_rom[12] = 16'd32710;
        sin_rom[13] = 16'd2615;
        cos_rom[13] = 16'd32700;
        sin_rom[14] = 16'd2816;
        cos_rom[14] = 16'd32689;
        sin_rom[15] = 16'd3017;
        cos_rom[15] = 16'd32677;
        sin_rom[16] = 16'd3217;
        cos_rom[16] = 16'd32665;
        sin_rom[17] = 16'd3417;
        cos_rom[17] = 16'd32651;
        sin_rom[18] = 16'd3617;
        cos_rom[18] = 16'd32636;
        sin_rom[19] = 16'd3817;
        cos_rom[19] = 16'd32620;
        sin_rom[20] = 16'd4017;
        cos_rom[20] = 16'd32603;
        sin_rom[21] = 16'd4217;
        cos_rom[21] = 16'd32585;
        sin_rom[22] = 16'd4416;
        cos_rom[22] = 16'd32566;
        sin_rom[23] = 16'd4615;
        cos_rom[23] = 16'd32546;
        sin_rom[24] = 16'd4813;
        cos_rom[24] = 16'd32525;
        sin_rom[25] = 16'd5011;
        cos_rom[25] = 16'd32503;
        sin_rom[26] = 16'd5208;
        cos_rom[26] = 16'd32479;
        sin_rom[27] = 16'd5405;
        cos_rom[27] = 16'd32455;
        sin_rom[28] = 16'd5601;
        cos_rom[28] = 16'd32429;
        sin_rom[29] = 16'd5796;
        cos_rom[29] = 16'd32402;
        sin_rom[30] = 16'd5991;
        cos_rom[30] = 16'd32374;
        sin_rom[31] = 16'd6185;
        cos_rom[31] = 16'd32345;
        sin_rom[32] = 16'd6378;
        cos_rom[32] = 16'd32315;
        sin_rom[33] = 16'd6571;
        cos_rom[33] = 16'd32284;
        sin_rom[34] = 16'd6762;
        cos_rom[34] = 16'd32251;
        sin_rom[35] = 16'd6952;
        cos_rom[35] = 16'd32217;
        sin_rom[36] = 16'd7142;
        cos_rom[36] = 16'd32182;
        sin_rom[37] = 16'd7330;
        cos_rom[37] = 16'd32146;
        sin_rom[38] = 16'd7518;
        cos_rom[38] = 16'd32109;
        sin_rom[39] = 16'd7704;
        cos_rom[39] = 16'd32070;
        sin_rom[40] = 16'd7890;
        cos_rom[40] = 16'd32030;
        sin_rom[41] = 16'd8074;
        cos_rom[41] = 16'd31989;
        sin_rom[42] = 16'd8257;
        cos_rom[42] = 16'd31947;
        sin_rom[43] = 16'd8439;
        cos_rom[43] = 16'd31903;
        sin_rom[44] = 16'd8619;
        cos_rom[44] = 16'd31858;
        sin_rom[45] = 16'd8799;
        cos_rom[45] = 16'd31812;
        sin_rom[46] = 16'd8977;
        cos_rom[46] = 16'd31765;
        sin_rom[47] = 16'd9154;
        cos_rom[47] = 16'd31716;
        sin_rom[48] = 16'd9330;
        cos_rom[48] = 16'd31666;
        sin_rom[49] = 16'd9504;
        cos_rom[49] = 16'd31615;
        sin_rom[50] = 16'd9677;
        cos_rom[50] = 16'd31563;
        sin_rom[51] = 16'd9849;
        cos_rom[51] = 16'd31509;
        sin_rom[52] = 16'd10019;
        cos_rom[52] = 16'd31455;
        sin_rom[53] = 16'd10188;
        cos_rom[53] = 16'd31398;
        sin_rom[54] = 16'd10355;
        cos_rom[54] = 16'd31341;
        sin_rom[55] = 16'd10521;
        cos_rom[55] = 16'd31282;
        sin_rom[56] = 16'd10685;
        cos_rom[56] = 16'd31222;
        sin_rom[57] = 16'd10848;
        cos_rom[57] = 16'd31160;
        sin_rom[58] = 16'd11009;
        cos_rom[58] = 16'd31098;
        sin_rom[59] = 16'd11168;
        cos_rom[59] = 16'd31034;
        sin_rom[60] = 16'd11326;
        cos_rom[60] = 16'd30969;
        sin_rom[61] = 16'd11482;
        cos_rom[61] = 16'd30903;
        sin_rom[62] = 16'd11636;
        cos_rom[62] = 16'd30836;
        sin_rom[63] = 16'd11789;
        cos_rom[63] = 16'd30767;
        sin_rom[64] = 16'd11940;
        cos_rom[64] = 16'd30697;
        sin_rom[65] = 16'd12089;
        cos_rom[65] = 16'd30626;
        sin_rom[66] = 16'd12236;
        cos_rom[66] = 16'd30554;
        sin_rom[67] = 16'd12382;
        cos_rom[67] = 16'd30480;
        sin_rom[68] = 16'd12526;
        cos_rom[68] = 16'd30405;
        sin_rom[69] = 16'd12668;
        cos_rom[69] = 16'd30328;
        sin_rom[70] = 16'd12808;
        cos_rom[70] = 16'd30250;
        sin_rom[71] = 16'd12946;
        cos_rom[71] = 16'd30171;
        sin_rom[72] = 16'd13082;
        cos_rom[72] = 16'd30090;
        sin_rom[73] = 16'd13217;
        cos_rom[73] = 16'd30008;
        sin_rom[74] = 16'd13349;
        cos_rom[74] = 16'd29924;
        sin_rom[75] = 16'd13480;
        cos_rom[75] = 16'd29839;
        sin_rom[76] = 16'd13608;
        cos_rom[76] = 16'd29753;
        sin_rom[77] = 16'd13735;
        cos_rom[77] = 16'd29665;
        sin_rom[78] = 16'd13860;
        cos_rom[78] = 16'd29576;
        sin_rom[79] = 16'd13982;
        cos_rom[79] = 16'd29486;
        sin_rom[80] = 16'd14103;
        cos_rom[80] = 16'd29394;
        sin_rom[81] = 16'd14221;
        cos_rom[81] = 16'd29301;
        sin_rom[82] = 16'd14337;
        cos_rom[82] = 16'd29206;
        sin_rom[83] = 16'd14451;
        cos_rom[83] = 16'd29110;
        sin_rom[84] = 16'd14563;
        cos_rom[84] = 16'd29013;
        sin_rom[85] = 16'd14673;
        cos_rom[85] = 16'd28914;
        sin_rom[86] = 16'd14780;
        cos_rom[86] = 16'd28814;
        sin_rom[87] = 16'd14886;
        cos_rom[87] = 16'd28712;
        sin_rom[88] = 16'd14989;
        cos_rom[88] = 16'd28609;
        sin_rom[89] = 16'd15090;
        cos_rom[89] = 16'd28505;
        sin_rom[90] = 16'd15189;
        cos_rom[90] = 16'd28399;
        sin_rom[91] = 16'd15286;
        cos_rom[91] = 16'd28292;
        sin_rom[92] = 16'd15380;
        cos_rom[92] = 16'd28183;
        sin_rom[93] = 16'd15472;
        cos_rom[93] = 16'd28073;
        sin_rom[94] = 16'd15562;
        cos_rom[94] = 16'd27962;
        sin_rom[95] = 16'd15650;
        cos_rom[95] = 16'd27849;
        sin_rom[96] = 16'd15735;
        cos_rom[96] = 16'd27735;
        sin_rom[97] = 16'd15818;
        cos_rom[97] = 16'd27620;
        sin_rom[98] = 16'd15899;
        cos_rom[98] = 16'd27503;
        sin_rom[99] = 16'd15978;
        cos_rom[99] = 16'd27385;
        sin_rom[100] = 16'd16054;
        cos_rom[100] = 16'd27265;
        sin_rom[101] = 16'd16128;
        cos_rom[101] = 16'd27144;
        sin_rom[102] = 16'd16200;
        cos_rom[102] = 16'd27022;
        sin_rom[103] = 16'd16269;
        cos_rom[103] = 16'd26898;
        sin_rom[104] = 16'd16336;
        cos_rom[104] = 16'd26773;
        sin_rom[105] = 16'd16401;
        cos_rom[105] = 16'd26647;
        sin_rom[106] = 16'd16463;
        cos_rom[106] = 16'd26519;
        sin_rom[107] = 16'd16523;
        cos_rom[107] = 16'd26390;
        sin_rom[108] = 16'd16580;
        cos_rom[108] = 16'd26259;
        sin_rom[109] = 16'd16635;
        cos_rom[109] = 16'd26127;
        sin_rom[110] = 16'd16688;
        cos_rom[110] = 16'd25993;
        sin_rom[111] = 16'd16738;
        cos_rom[111] = 16'd25858;
        sin_rom[112] = 16'd16785;
        cos_rom[112] = 16'd25721;
        sin_rom[113] = 16'd16830;
        cos_rom[113] = 16'd25583;
        sin_rom[114] = 16'd16872;
        cos_rom[114] = 16'd25444;
        sin_rom[115] = 16'd16912;
        cos_rom[115] = 16'd25302;
        sin_rom[116] = 16'd16949;
        cos_rom[116] = 16'd25160;
        sin_rom[117] = 16'd16983;
        cos_rom[117] = 16'd25016;
        sin_rom[118] = 16'd17015;
        cos_rom[118] = 16'd24870;
        sin_rom[119] = 16'd17044;
        cos_rom[119] = 16'd24723;
        sin_rom[120] = 16'd17070;
        cos_rom[120] = 16'd24575;
        sin_rom[121] = 16'd17093;
        cos_rom[121] = 16'd24425;
        sin_rom[122] = 16'd17114;
        cos_rom[122] = 16'd24274;
        sin_rom[123] = 16'd17132;
        cos_rom[123] = 16'd24121;
        sin_rom[124] = 16'd17147;
        cos_rom[124] = 16'd23967;
        sin_rom[125] = 16'd17159;
        cos_rom[125] = 16'd23812;
        sin_rom[126] = 16'd17169;
        cos_rom[126] = 16'd23655;
        sin_rom[127] = 16'd17175;
        cos_rom[127] = 16'd23497;
        sin_rom[128] = 16'd17177;
        cos_rom[128] = 16'd23338;
        sin_rom[129] = 16'd17175;
        cos_rom[129] = 16'd23178;
        sin_rom[130] = 16'd17169;
        cos_rom[130] = 16'd23016;
        sin_rom[131] = 16'd17159;
        cos_rom[131] = 16'd22854;
        sin_rom[132] = 16'd17147;
        cos_rom[132] = 16'd22690;
        sin_rom[133] = 16'd17132;
        cos_rom[133] = 16'd22525;
        sin_rom[134] = 16'd17114;
        cos_rom[134] = 16'd22359;
        sin_rom[135] = 16'd17093;
        cos_rom[135] = 16'd22191;
        sin_rom[136] = 16'd17070;
        cos_rom[136] = 16'd22022;
        sin_rom[137] = 16'd17044;
        cos_rom[137] = 16'd21852;
        sin_rom[138] = 16'd17015;
        cos_rom[138] = 16'd21680;
        sin_rom[139] = 16'd16983;
        cos_rom[139] = 16'd21508;
        sin_rom[140] = 16'd16949;
        cos_rom[140] = 16'd21334;
        sin_rom[141] = 16'd16912;
        cos_rom[141] = 16'd21159;
        sin_rom[142] = 16'd16872;
        cos_rom[142] = 16'd20982;
        sin_rom[143] = 16'd16830;
        cos_rom[143] = 16'd20805;
        sin_rom[144] = 16'd16785;
        cos_rom[144] = 16'd20626;
        sin_rom[145] = 16'd16738;
        cos_rom[145] = 16'd20446;
        sin_rom[146] = 16'd16688;
        cos_rom[146] = 16'd20265;
        sin_rom[147] = 16'd16635;
        cos_rom[147] = 16'd20083;
        sin_rom[148] = 16'd16580;
        cos_rom[148] = 16'd19900;
        sin_rom[149] = 16'd16523;
        cos_rom[149] = 16'd19715;
        sin_rom[150] = 16'd16463;
        cos_rom[150] = 16'd19529;
        sin_rom[151] = 16'd16401;
        cos_rom[151] = 16'd19342;
        sin_rom[152] = 16'd16336;
        cos_rom[152] = 16'd19154;
        sin_rom[153] = 16'd16269;
        cos_rom[153] = 16'd18965;
        sin_rom[154] = 16'd16200;
        cos_rom[154] = 16'd18774;
        sin_rom[155] = 16'd16128;
        cos_rom[155] = 16'd18582;
        sin_rom[156] = 16'd16054;
        cos_rom[156] = 16'd18389;
        sin_rom[157] = 16'd15978;
        cos_rom[157] = 16'd18195;
        sin_rom[158] = 16'd15899;
        cos_rom[158] = 16'd18000;
        sin_rom[159] = 16'd15818;
        cos_rom[159] = 16'd17803;
        sin_rom[160] = 16'd15735;
        cos_rom[160] = 16'd17606;
        sin_rom[161] = 16'd15650;
        cos_rom[161] = 16'd17407;
        sin_rom[162] = 16'd15562;
        cos_rom[162] = 16'd17207;
        sin_rom[163] = 16'd15472;
        cos_rom[163] = 16'd17007;
        sin_rom[164] = 16'd15380;
        cos_rom[164] = 16'd16805;
        sin_rom[165] = 16'd15286;
        cos_rom[165] = 16'd16602;
        sin_rom[166] = 16'd15189;
        cos_rom[166] = 16'd16398;
        sin_rom[167] = 16'd15090;
        cos_rom[167] = 16'd16193;
        sin_rom[168] = 16'd14989;
        cos_rom[168] = 16'd15987;
        sin_rom[169] = 16'd14886;
        cos_rom[169] = 16'd15780;
        sin_rom[170] = 16'd14780;
        cos_rom[170] = 16'd15572;
        sin_rom[171] = 16'd14673;
        cos_rom[171] = 16'd15363;
        sin_rom[172] = 16'd14563;
        cos_rom[172] = 16'd15152;
        sin_rom[173] = 16'd14451;
        cos_rom[173] = 16'd14940;
        sin_rom[174] = 16'd14337;
        cos_rom[174] = 16'd14728;
        sin_rom[175] = 16'd14221;
        cos_rom[175] = 16'd14514;
        sin_rom[176] = 16'd14103;
        cos_rom[176] = 16'd14299;
        sin_rom[177] = 16'd13982;
        cos_rom[177] = 16'd14083;
        sin_rom[178] = 16'd13860;
        cos_rom[178] = 16'd13866;
        sin_rom[179] = 16'd13735;
        cos_rom[179] = 16'd13647;
        sin_rom[180] = 16'd13608;
        cos_rom[180] = 16'd13428;
        sin_rom[181] = 16'd13480;
        cos_rom[181] = 16'd13207;
        sin_rom[182] = 16'd13349;
        cos_rom[182] = 16'd12985;
        sin_rom[183] = 16'd13217;
        cos_rom[183] = 16'd12761;
        sin_rom[184] = 16'd13082;
        cos_rom[184] = 16'd12536;
        sin_rom[185] = 16'd12946;
        cos_rom[185] = 16'd12310;
        sin_rom[186] = 16'd12808;
        cos_rom[186] = 16'd12082;
        sin_rom[187] = 16'd12668;
        cos_rom[187] = 16'd11853;
        sin_rom[188] = 16'd12526;
        cos_rom[188] = 16'd11623;
        sin_rom[189] = 16'd12382;
        cos_rom[189] = 16'd11392;
        sin_rom[190] = 16'd12236;
        cos_rom[190] = 16'd11159;
        sin_rom[191] = 16'd12089;
        cos_rom[191] = 16'd10925;
        sin_rom[192] = 16'd11940;
        cos_rom[192] = 16'd10690;
        sin_rom[193] = 16'd11789;
        cos_rom[193] = 16'd10453;
        sin_rom[194] = 16'd11636;
        cos_rom[194] = 16'd10215;
        sin_rom[195] = 16'd11482;
        cos_rom[195] = 16'd9976;
        sin_rom[196] = 16'd11326;
        cos_rom[196] = 16'd9735;
        sin_rom[197] = 16'd11168;
        cos_rom[197] = 16'd9494;
        sin_rom[198] = 16'd11009;
        cos_rom[198] = 16'd9251;
        sin_rom[199] = 16'd10848;
        cos_rom[199] = 16'd9007;
        sin_rom[200] = 16'd10685;
        cos_rom[200] = 16'd8762;
        sin_rom[201] = 16'd10521;
        cos_rom[201] = 16'd8516;
        sin_rom[202] = 16'd10355;
        cos_rom[202] = 16'd8269;
        sin_rom[203] = 16'd10188;
        cos_rom[203] = 16'd8020;
        sin_rom[204] = 16'd10019;
        cos_rom[204] = 16'd7770;
        sin_rom[205] = 16'd9849;
        cos_rom[205] = 16'd7518;
        sin_rom[206] = 16'd9677;
        cos_rom[206] = 16'd7266;
        sin_rom[207] = 16'd9504;
        cos_rom[207] = 16'd7012;
        sin_rom[208] = 16'd9330;
        cos_rom[208] = 16'd6757;
        sin_rom[209] = 16'd9154;
        cos_rom[209] = 16'd6501;
        sin_rom[210] = 16'd8977;
        cos_rom[210] = 16'd6244;
        sin_rom[211] = 16'd8799;
        cos_rom[211] = 16'd5986;
        sin_rom[212] = 16'd8619;
        cos_rom[212] = 16'd5727;
        sin_rom[213] = 16'd8439;
        cos_rom[213] = 16'd5467;
        sin_rom[214] = 16'd8257;
        cos_rom[214] = 16'd5206;
        sin_rom[215] = 16'd8074;
        cos_rom[215] = 16'd4944;
        sin_rom[216] = 16'd7890;
        cos_rom[216] = 16'd4681;
        sin_rom[217] = 16'd7704;
        cos_rom[217] = 16'd4417;
        sin_rom[218] = 16'd7518;
        cos_rom[218] = 16'd4153;
        sin_rom[219] = 16'd7330;
        cos_rom[219] = 16'd3887;
        sin_rom[220] = 16'd7142;
        cos_rom[220] = 16'd3620;
        sin_rom[221] = 16'd6952;
        cos_rom[221] = 16'd3353;
        sin_rom[222] = 16'd6762;
        cos_rom[222] = 16'd3085;
        sin_rom[223] = 16'd6571;
        cos_rom[223] = 16'd2817;
        sin_rom[224] = 16'd6378;
        cos_rom[224] = 16'd2547;
        sin_rom[225] = 16'd6185;
        cos_rom[225] = 16'd2277;
        sin_rom[226] = 16'd5991;
        cos_rom[226] = 16'd2006;
        sin_rom[227] = 16'd5796;
        cos_rom[227] = 16'd1735;
        sin_rom[228] = 16'd5601;
        cos_rom[228] = 16'd1463;
        sin_rom[229] = 16'd5405;
        cos_rom[229] = 16'd1191;
        sin_rom[230] = 16'd5208;
        cos_rom[230] = 16'd919;
        sin_rom[231] = 16'd5011;
        cos_rom[231] = 16'd646;
        sin_rom[232] = 16'd4813;
        cos_rom[232] = 16'd373;
        sin_rom[233] = 16'd4615;
        cos_rom[233] = 16'd100;
        sin_rom[234] = 16'd4416;
        cos_rom[234] = 16'hFF54; // -172
        sin_rom[235] = 16'd4217;
        cos_rom[235] = 16'hFE43; // -445
        sin_rom[236] = 16'd4017;
        cos_rom[236] = 16'hFD32; // -718
        sin_rom[237] = 16'd3817;
        cos_rom[237] = 16'hFC21; // -991
        sin_rom[238] = 16'd3617;
        cos_rom[238] = 16'hFB00; // -1264
        sin_rom[239] = 16'd3417;
        cos_rom[239] = 16'hF9F0; // -1536
        sin_rom[240] = 16'd3217;
        cos_rom[240] = 16'hF8F0; // -1808
        sin_rom[241] = 16'd3017;
        cos_rom[241] = 16'hF7E0; // -2080
        sin_rom[242] = 16'd2816;
        cos_rom[242] = 16'hF6D0; // -2352
        sin_rom[243] = 16'd2615;
        cos_rom[243] = 16'hF5C0; // -2623
        sin_rom[244] = 16'd2414;
        cos_rom[244] = 16'hF4C0; // -2893
        sin_rom[245] = 16'd2213;
        cos_rom[245] = 16'hF3B0; // -3163
        sin_rom[246] = 16'd2012;
        cos_rom[246] = 16'hF2A0; // -3432
        sin_rom[247] = 16'd1811;
        cos_rom[247] = 16'hF190; // -3700
        sin_rom[248] = 16'd1610;
        cos_rom[248] = 16'hF080; // -3967
        sin_rom[249] = 16'd1409;
        cos_rom[249] = 16'hEF70; // -4233
        sin_rom[250] = 16'd1208;
        cos_rom[250] = 16'hEE60; // -4498
        sin_rom[251] = 16'd1006;
        cos_rom[251] = 16'hED50; // -4762
        sin_rom[252] = 16'd805;
        cos_rom[252] = 16'hEC40; // -5025
        sin_rom[253] = 16'd604;
        cos_rom[253] = 16'hEB30; // -5286
        sin_rom[254] = 16'd402;
        cos_rom[254] = 16'hEA20; // -5546
        sin_rom[255] = 16'd201;
        cos_rom[255] = 16'hE910; // -5805

    end
    
    // ========================================================================
    // Phase accumulator
    // ========================================================================
    
    reg [31:0] phase_accumulator;
    reg [31:0] phase_accumulator_next;
    
    // Phase increment based on frequency word
    wire [31:0] phase_increment;
    assign phase_increment = enable ? frequency_word : 32'd0;
    
    // Phase accumulator register
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            phase_accumulator <= 32'd0;
        end else begin
            phase_accumulator <= phase_accumulator_next;
        end
    end
    
    // Next phase calculation
    assign phase_accumulator_next = phase_accumulator + phase_increment;
    
    // ========================================================================
    // Phase to ROM address mapping
    // ========================================================================
    
    // Extract most significant bits for ROM addressing
    wire [7:0] phase_upper;
    assign phase_upper = phase_accumulator[31:24];
    
    // Determine quadrant for sine/cosine calculation
    wire [1:0] quadrant;
    assign quadrant = phase_accumulator[31:30];
    
    // ROM address (quarter sine wave table)
    wire [7:0] rom_address;
    assign rom_address = phase_accumulator[23:16];
    
    // ========================================================================
    // Sine and cosine calculation
    // ========================================================================
    
    reg [15:0] sin_raw;
    reg [15:0] cos_raw;
    
    // ROM lookup with quadrant correction
    always @(*) begin
        case (quadrant)
            2'b00: begin // 0 to 90 degrees
                sin_raw = sin_rom[rom_address];
                cos_raw = cos_rom[rom_address];
            end
            2'b01: begin // 90 to 180 degrees
                sin_raw = cos_rom[ROM_DEPTH - 1 - rom_address];
                cos_raw = -sin_rom[ROM_DEPTH - 1 - rom_address];
            end
            2'b10: begin // 180 to 270 degrees
                sin_raw = -sin_rom[rom_address];
                cos_raw = -cos_rom[rom_address];
            end
            2'b11: begin // 270 to 360 degrees
                sin_raw = -cos_rom[ROM_DEPTH - 1 - rom_address];
                cos_raw = sin_rom[ROM_DEPTH - 1 - rom_address];
            end
            default: begin
                sin_raw = 16'd0;
                cos_raw = 16'd0;
            end
        endcase
    end
    
    // ========================================================================
    // Output register and scaling
    // ========================================================================
    
    reg [15:0] sine_reg;
    reg [15:0] cosine_reg;
    reg        valid_reg;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sine_reg <= 16'd0;
            cosine_reg <= 16'd0;
            valid_reg <= 1'b0;
        end else begin
            if (enable) begin
                sine_reg <= sin_raw;
                cosine_reg <= cos_raw;
                valid_reg <= 1'b1;
            end else begin
                valid_reg <= 1'b0;
            end
        end
    end
    
    // Output assignment
    assign sine_out = sine_reg;
    assign cosine_out = cosine_reg;
    assign valid_out = valid_reg;
    
    // ========================================================================
    // Spurious response suppression (optional)
    // ========================================================================
    
    // Phase dithering to reduce spurious responses
    reg [31:0] dither_register;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            dither_register <= 32'h55555555; // Alternating pattern
        end else begin
            dither_register <= {dither_register[30:0], dither_register[31]}; // Rotate
        end
    end
    
    // Apply dithering to frequency word (optional feature)
    wire [31:0] dithered_frequency;
    assign dithered_frequency = frequency_word ^ dither_register[7:0];
    
    // Use dithered frequency word
    wire [31:0] effective_frequency;
    assign effective_frequency = dithered_frequency;

endmodule
