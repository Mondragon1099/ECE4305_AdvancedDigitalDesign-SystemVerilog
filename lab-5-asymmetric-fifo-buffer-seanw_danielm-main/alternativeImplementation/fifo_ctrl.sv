`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/25/2026 12:32:35 AM
// Design Name: 
// Module Name: fifo_ctrl
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module fifo_ctrl
    #(parameter ADDR_WIDTH = 3)
    (
        input  logic                      clk, reset,
        input  logic                      wr, rd,
        output logic                      full, empty,
        output logic [ADDR_WIDTH - 1 : 0] w_addr,
        output logic [ADDR_WIDTH     : 0] r_addr
    );

    logic [ADDR_WIDTH - 1 : 0] wr_ptr, wr_ptr_next;
    logic [ADDR_WIDTH     : 0] rd_ptr, rd_ptr_next;
    logic full_next, empty_next;

    // registers
    always_ff @(posedge clk, posedge reset) begin
        if (reset) begin
            wr_ptr <= 0;
            rd_ptr <= 0;
            full   <= 1'b0;
            empty  <= 1'b1;
        end else begin
            wr_ptr <= wr_ptr_next;
            rd_ptr <= rd_ptr_next;
            full   <= full_next;
            empty  <= empty_next;
        end
    end

    always_comb begin
        wr_ptr_next = wr_ptr;
        rd_ptr_next = rd_ptr;
        full_next   = full;
        empty_next  = empty;

        unique case ({wr, rd})
            2'b01: // read only
            begin
                if (~empty) begin
                    rd_ptr_next = rd_ptr + 1;
                    full_next   = 1'b0;
                    // empty when rd_ptr catches up to write pointer (scaled to read space)
                    if (rd_ptr_next == {wr_ptr, 1'b0})
                        empty_next = 1'b1;
                end
            end

            2'b10: // write only
            begin
                if (~full) begin
                    wr_ptr_next = wr_ptr + 1;
                    empty_next  = 1'b0;
                    // full when write pointer catches up to read pointer (scaled down)
                    if (wr_ptr_next == rd_ptr[ADDR_WIDTH : 1])
                        full_next = 1'b1;
                end
            end

            2'b11: // simultaneous read and write
            begin
                if (empty) begin
                    wr_ptr_next = wr_ptr;
                    rd_ptr_next = rd_ptr;
                end else begin
                    wr_ptr_next = wr_ptr + 1;
                    rd_ptr_next = rd_ptr + 1;
                    // re-evaluate full/empty after both move
                    if (wr_ptr_next == rd_ptr_next[ADDR_WIDTH : 1])
                        full_next = 1'b1;
                    else
                        full_next = 1'b0;
                    if (rd_ptr_next == {wr_ptr_next, 1'b0})
                        empty_next = 1'b1;
                    else
                        empty_next = 1'b0;
                end
            end

            default: ; // 2'b00 no-op
        endcase
    end

    assign w_addr = wr_ptr;
    assign r_addr = rd_ptr;

endmodule
