// ============================================================================
// v2_phy_manager - KSZ9031 bring-up over MDIO, for the Code-SDR V2 board.
//
// Rewritten with an explicit "operation" model: each transaction's register
// address, direction AND data all come from the SAME operation, and are latched
// into registers at launch. The previous version derived the address from the
// *next* state while deriving the data from the *current* state, so every
// transaction was off by one operation (it wrote GBCTL into BMCR and never
// issued the BMCR reset at all).
//
// Sequence:
//   read ID1 -> read ID2 -> write BMCR reset -> poll BMCR bit15
//   -> 4x MMD RGMII pad-skew writes (dev 2: regs 0x04,0x05,0x06,0x08)
//   -> write GBCTL (advertise 1000BASE-T FD) -> write BMCR (AN enable + restart)
//   -> poll BMSR link -> read GBSTAT -> read BMSR x2 -> ready
//   -> then re-read BMSR on a slow timer (and on phy_int_n) so link_up keeps
//      tracking the cable instead of freezing at the bring-up value.
//
// Two-phase per operation: LAUNCH (drive start + latch op) then WAIT (md_done,
// then check the result and pick the next operation).
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_phy_manager #(
    parameter integer SKEW_TX_DATA = 7,
    parameter integer SKEW_TX_CTL  = 7,
    parameter integer SKEW_TX_CLK  = 15,
    parameter integer SKEW_RX_DATA = 7,
    parameter integer SKEW_RX_CTL  = 7,
    parameter integer SKEW_RX_CLK  = 15
) (
    input  wire clk,
    input  wire rst_n,
    input  wire phy_int_n,
    input  wire mdio_i,
    output wire mdc,
    output reg  mdio_o,
    output reg  mdio_oe,
    output reg  link_up,
    output reg  ready,
    output reg  error
);

    // ------------------------------------------------------------- register map
    localparam [4:0] PHY_ADDR      = 5'd0;
    localparam [4:0] REG_BMCR      = 5'd0;
    localparam [4:0] REG_BMSR      = 5'd1;
    localparam [4:0] REG_ID1       = 5'd2;
    localparam [4:0] REG_ID2       = 5'd3;
    localparam [4:0] REG_GBCTL     = 5'd9;
    localparam [4:0] REG_GBSTAT    = 5'd10;
    localparam [4:0] REG_MMD_CTRL  = 5'd13;
    localparam [4:0] REG_MMD_DATA  = 5'd14;

    localparam [15:0] KSZ_ID1        = 16'h0022;
    localparam [15:0] KSZ_ID2        = 16'h1620;
    localparam [15:0] KSZ_ID2_MASK   = 16'hfff0;
    localparam [15:0] BMCR_RESET     = 16'h8000;
    localparam [15:0] BMCR_ANEN      = 16'h1200;   // AN enable + restart AN
    localparam [15:0] GBCTL_1000FD   = 16'h0300;   // advertise 1000BASE-T FD
    localparam [15:0] MMD_DEV2       = 16'h0002;
    localparam [15:0] MMD_DATA_MODE  = 16'h4002;   // func=data, no post-inc
    localparam integer RESET_POLL_MAX = 64;
    localparam integer LINK_POLL_MAX  = 64;

    // MMD sub-registers (device 2)
    localparam [15:0] MMD_REG_CTRL_SKW = 16'h0004;
    localparam [15:0] MMD_REG_RX_DATA  = 16'h0005;
    localparam [15:0] MMD_REG_TX_DATA  = 16'h0006;
    localparam [15:0] MMD_REG_CLOCK    = 16'h0008;

    // -------------------------------------------------------------- operations
    localparam [4:0] O_ID1=5'd0, O_ID2=5'd1, O_RST=5'd2, O_CHKRST=5'd3,
                     O_MMDA=5'd4, O_MMDD=5'd5, O_MMDC=5'd6, O_MMDW=5'd7,
                     O_GBCTL=5'd8, O_BMCR=5'd9, O_ANP=5'd10, O_GBSTAT=5'd11,
                     O_BMSR1=5'd12, O_BMSR2=5'd13, O_DONE=5'd14, O_FAIL=5'd15,
                     O_MON=5'd16;

    reg [4:0]  op;          // current operation (launched / being waited on)
    reg        phase;       // 0 = launch, 1 = wait
    reg [15:0] poll_count;
    reg [4:0]  skew_index;
    reg [15:0] scratch;

    // ------------------------------------------------------------- MDIO master
    reg         start;
    reg         read_op;
    reg  [4:0]  md_reg_addr;
    reg  [15:0] write_data;
    wire [15:0] read_data;
    wire        md_busy, md_done, md_error;

    v2_mdio_master u_mdio (
        .clk(clk), .rst_n(rst_n), .start(start), .read_op(read_op),
        .phy_addr(PHY_ADDR), .reg_addr(md_reg_addr),
        .write_data(write_data), .read_data(read_data),
        .busy(md_busy), .done(md_done), .error(md_error), .mdc(mdc),
        .mdio_i(mdio_i), .mdio_o(mdio_o), .mdio_oe(mdio_oe)
    );

    // ------------------------------------------------------ per-op parameters
    function [4:0] op_addr_of;
        input [4:0] o;
        begin
            case (o)
                O_ID1:    op_addr_of = REG_ID1;
                O_ID2:    op_addr_of = REG_ID2;
                O_RST:    op_addr_of = REG_BMCR;
                O_CHKRST: op_addr_of = REG_BMCR;
                O_MMDA:   op_addr_of = REG_MMD_CTRL;
                O_MMDD:   op_addr_of = REG_MMD_DATA;
                O_MMDC:   op_addr_of = REG_MMD_CTRL;
                O_MMDW:   op_addr_of = REG_MMD_DATA;
                O_GBCTL:  op_addr_of = REG_GBCTL;
                O_BMCR:   op_addr_of = REG_BMCR;
                O_ANP:    op_addr_of = REG_BMSR;
                O_MON:    op_addr_of = REG_BMSR;
                O_GBSTAT: op_addr_of = REG_GBSTAT;
                default:  op_addr_of = REG_BMSR;   // BMSR1 / BMSR2
            endcase
        end
    endfunction

    function op_is_read;
        input [4:0] o;
        begin
            case (o)
                O_ID1, O_ID2, O_CHKRST, O_ANP, O_GBSTAT, O_BMSR1, O_BMSR2, O_MON:
                    op_is_read = 1'b1;
                default: op_is_read = 1'b0;
            endcase
        end
    endfunction

    // ---------------------------------------------------------- MMD skew value
    reg [15:0] skew_value;
    reg [15:0] mmd_reg;

    always @* begin
        case (skew_index)
            5'd0: begin
                skew_value = {4'b0000, SKEW_RX_CTL[3:0], SKEW_TX_CTL[3:0]};
                mmd_reg    = MMD_REG_CTRL_SKW;
            end
            5'd1: begin
                skew_value = {SKEW_RX_DATA[3:0], SKEW_RX_DATA[3:0],
                              SKEW_RX_DATA[3:0], SKEW_RX_DATA[3:0]};
                mmd_reg    = MMD_REG_RX_DATA;
            end
            5'd2: begin
                skew_value = {SKEW_TX_DATA[3:0], SKEW_TX_DATA[3:0],
                              SKEW_TX_DATA[3:0], SKEW_TX_DATA[3:0]};
                mmd_reg    = MMD_REG_TX_DATA;
            end
            default: begin
                skew_value = {6'b000000, SKEW_TX_CLK[4:0], SKEW_RX_CLK[4:0]};
                mmd_reg    = MMD_REG_CLOCK;
            end
        endcase
    end

    wire skew_last = (skew_index == 5'd3);

    // -------------------------------------------------- periodic link re-poll
    // The one-shot bring-up freezes link_up at whatever it saw; a later cable
    // unplug then leaves telemetry reporting a stale link. Re-read BMSR on a
    // slow timer, and once more just after the PHY's active-low interrupt
    // falls. BMSR bit 2 is latch-low, so the read itself re-arms the interrupt.
    localparam integer LINK_REPOLL_CYCLES = 250000;   // 2 ms at 125 MHz
    localparam integer LINK_TIMER_WIDTH = $clog2(LINK_REPOLL_CYCLES + 1);
    reg [LINK_TIMER_WIDTH-1:0] link_timer;
    reg phy_int_s0, phy_int_s1;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            link_timer <= {LINK_TIMER_WIDTH{1'b0}};
            phy_int_s0 <= 1'b1;
            phy_int_s1 <= 1'b1;
        end else begin
            phy_int_s0 <= phy_int_n;
            phy_int_s1 <= phy_int_s0;
            if (op == O_MON) link_timer <= {LINK_TIMER_WIDTH{1'b0}};
            else if (link_timer != LINK_REPOLL_CYCLES)
                link_timer <= link_timer + 1'b1;
        end
    end

    wire link_req       = (link_timer == LINK_REPOLL_CYCLES);
    wire phy_int_assert = phy_int_s1 & ~phy_int_s0;

    // ---------------------------------------------------------------- sequencer
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            op         <= O_ID1;
            phase      <= 1'b0;
            start      <= 1'b0;
            read_op    <= 1'b0;
            md_reg_addr<= REG_ID1;
            write_data <= 16'd0;
            poll_count <= 16'd0;
            skew_index <= 5'd0;
            scratch    <= 16'd0;
            link_up    <= 1'b0;
            ready      <= 1'b0;
            error      <= 1'b0;
        end else begin
            start <= 1'b0;

            if (!phase) begin
                // ----------------------------------------------------- launch
                if (op == O_DONE) begin
                    // Bring-up is complete, but the cable can still move. Kick a
                    // monitor read on the timer or a PHY interrupt edge.
                    if (link_req || phy_int_assert) op <= O_MON;
                end else if (op != O_FAIL) begin
                    // Address, direction and data all come from the same
                    // operation and are registered, so the master sees them
                    // stable the cycle after start is asserted.
                    read_op    <= op_is_read(op);
                    md_reg_addr<= op_addr_of(op);
                    write_data <= (op == O_RST)   ? BMCR_RESET   :
                                  (op == O_MMDA)  ? MMD_DEV2     :
                                  (op == O_MMDD)  ? mmd_reg      :
                                  (op == O_MMDC)  ? MMD_DATA_MODE:
                                  (op == O_MMDW)  ? skew_value   :
                                  (op == O_GBCTL) ? GBCTL_1000FD :
                                  (op == O_BMCR)  ? BMCR_ANEN    : 16'd0;
                    start   <= 1'b1;
                    phase   <= 1'b1;
                end
            end else if (!md_busy && !start && md_done) begin
                // -------------------------------------------------- WAIT: result
                case (op)
                    O_ID1: begin
                        if (read_data == KSZ_ID1) op <= O_ID2;
                        else begin error <= 1'b1; op <= O_FAIL; end
                    end
                    O_ID2: begin
                        if ((read_data & KSZ_ID2_MASK) == KSZ_ID2) op <= O_RST;
                        else begin error <= 1'b1; op <= O_FAIL; end
                    end
                    O_RST: begin
                        poll_count <= 16'd0;
                        op <= O_CHKRST;
                    end
                    O_CHKRST: begin
                        if (!read_data[15]) begin
                            skew_index <= 5'd0;
                            op <= O_MMDA;
                        end else if (poll_count == RESET_POLL_MAX) begin
                            error <= 1'b1; op <= O_FAIL;
                        end else begin
                            poll_count <= poll_count + 16'd1;
                            op <= O_CHKRST;
                        end
                    end
                    O_MMDA: op <= O_MMDD;
                    O_MMDD: op <= O_MMDC;
                    O_MMDC: op <= O_MMDW;
                    O_MMDW: begin
                        if (skew_last) op <= O_GBCTL;
                        else begin skew_index <= skew_index + 5'd1; op <= O_MMDA; end
                    end
                    O_GBCTL: op <= O_BMCR;
                    O_BMCR: begin
                        poll_count <= 16'd0;
                        op <= O_ANP;
                    end
                    O_ANP: begin
                        if (read_data[2]) begin
                            op <= O_GBSTAT;
                        end else if (poll_count == LINK_POLL_MAX) begin
                            op <= O_GBSTAT;      // give up polling, report status
                        end else begin
                            poll_count <= poll_count + 16'd1;
                            op <= O_ANP;
                        end
                    end
                    O_GBSTAT: begin
                        link_up <= read_data[2];
                        op <= O_BMSR1;
                    end
                    O_BMSR1: begin
                        scratch <= read_data;
                        op <= O_BMSR2;
                    end
                    O_BMSR2: begin
                        link_up <= scratch[2] & read_data[2];
                        ready   <= 1'b1;
                        op      <= O_DONE;
                    end
                    O_MON: begin
                        link_up <= read_data[2];
                        op      <= O_DONE;
                    end
                    O_DONE, O_FAIL: ;
                    default: op <= O_ID1;
                endcase
                phase <= 1'b0;
            end
        end
    end

endmodule

`default_nettype wire
