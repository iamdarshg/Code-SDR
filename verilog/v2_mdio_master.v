`timescale 1ns/1ps
`default_nettype none

module v2_mdio_master #(
    parameter integer HALF_PERIOD = 20
) (
    input wire clk,
    input wire rst_n,
    input wire start,
    input wire read_op,
    input wire [4:0] phy_addr,
    input wire [4:0] reg_addr,
    input wire [15:0] write_data,
    output reg [15:0] read_data,
    output reg busy,
    output reg done,
    output reg error,
    output reg mdc,
    input wire mdio_i,
    output wire mdio_o,
    output wire mdio_oe
);
    localparam integer DIV_WIDTH = HALF_PERIOD > 1 ? $clog2(HALF_PERIOD) : 1;
    reg [DIV_WIDTH-1:0] divider;
    reg [63:0] frame;
    reg [5:0] bit_index;
    reg reading;

    assign mdio_o = frame[63];
    assign mdio_oe = busy && !(reading && bit_index >= 6'd46);

    initial begin
        if (HALF_PERIOD < 20)
            $fatal(1, "MDC must not exceed 2.5 MHz at a 100 MHz input");
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            divider <= 0;
            frame <= 64'hffffffffffffffff;
            bit_index <= 0;
            reading <= 0;
            read_data <= 0;
            busy <= 0;
            done <= 0;
            error <= 0;
            mdc <= 0;
        end else begin
            done <= 0;
            if (!busy) begin
                mdc <= 0;
                divider <= 0;
                if (start) begin
                    frame <= {32'hffffffff, 2'b01, read_op ? 2'b10 : 2'b01,
                              phy_addr, reg_addr, 2'b10, write_data};
                    reading <= read_op;
                    bit_index <= 0;
                    read_data <= 0;
                    error <= 0;
                    busy <= 1;
                end
            end else if (divider == HALF_PERIOD - 1) begin
                divider <= 0;
                mdc <= !mdc;
                if (!mdc) begin
                    if (reading && bit_index == 6'd47 && mdio_i !== 1'b0)
                        error <= 1;
                    if (reading && bit_index >= 6'd48)
                        read_data <= {read_data[14:0], mdio_i};
                end else if (bit_index == 6'd63) begin
                    busy <= 0;
                    done <= 1;
                end else begin
                    bit_index <= bit_index + 1'b1;
                    frame <= {frame[62:0], 1'b1};
                end
            end else begin
                divider <= divider + 1'b1;
            end
        end
    end
endmodule

`default_nettype wire
