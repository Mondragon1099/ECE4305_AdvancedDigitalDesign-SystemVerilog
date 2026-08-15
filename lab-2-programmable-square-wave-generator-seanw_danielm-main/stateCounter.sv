`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Daniel and Sean
// 
// Create Date: 02/02/2026 05:18:37 PM
// Design Name: Programmable Square Wave Generator
// Module Name: squareWaveGen
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


module squareWaveGen(
    input logic [3:0] M, N, // switches [0:7]
    input logic clk, reset, // resetN from XDC file
    output logic out_put // Pmod JA[1]
);
    localparam WIDTH = 8;
    
    // Internal signals
    logic [WIDTH:0] r_reg;      // Current count
    logic [WIDTH:0] r_next;     // Next count
    
    // Module 1: combinational logic
    stateCounter #(.WIDTH(WIDTH)) counter (
        .M(M),
        .N(N),
        .current_count(r_reg),
        .next_count(r_next),
        .out_put(out_put)
    );
    
    // Module 2: D Flip-Flop register (sequential logic)
    dff #(.WIDTH(WIDTH)) register (
        .clk(clk),
        .reset(reset),
        .d(r_next),
        .q(r_reg)
    );
    
endmodule

