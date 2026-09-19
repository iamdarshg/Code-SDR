// ============================================================================
// tb_utils.vh - helpers shared by the Code-SDR V2 testbenches.
//
// `include this INSIDE a module body (it defines functions, not a module).
//
// Extracted because crc32_byte was copy-pasted verbatim into three testbenches;
// a single copy means a fix or a format change can only be made once.
// ============================================================================

// Ethernet FCS, byte at a time: CRC-32 (IEEE 802.3), reflected, init
// 0xFFFFFFFF, final inversion left to the caller. Known-good check:
// the FCS of the ASCII string "123456789" is 0xCBF43926.
function [31:0] crc32_byte;
    input [31:0] c_in;
    input [7:0]  d;
    integer i;
    reg [31:0] x;
    begin
        x = c_in ^ {24'h0, d};
        for (i = 0; i < 8; i = i + 1)
            x = x[0] ? ((x >> 1) ^ 32'hEDB88320) : (x >> 1);
        crc32_byte = x;
    end
endfunction
