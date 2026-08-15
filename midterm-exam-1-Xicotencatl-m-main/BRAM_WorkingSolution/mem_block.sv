`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/27/2022 07:29:58 PM
// Design Name: 
// Module Name: mem_block
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


module mem_block(
    input  logic        clk,
    input  logic        we,
    input  logic [11:0] addr,
    input  logic [3:0]  din,
    output logic [3:0]  dout
);
    logic [3:0] dout_bank [0:3];

    // only enable the selected bank
    mem_bank bank0(.clk(clk), .we(we && addr[11:10]==2'b00), .addr(addr[9:0]), .din(din), .dout(dout_bank[0]));
    mem_bank bank1(.clk(clk), .we(we && addr[11:10]==2'b01), .addr(addr[9:0]), .din(din), .dout(dout_bank[1]));
    mem_bank bank2(.clk(clk), .we(we && addr[11:10]==2'b10), .addr(addr[9:0]), .din(din), .dout(dout_bank[2]));
    mem_bank bank3(.clk(clk), .we(we && addr[11:10]==2'b11), .addr(addr[9:0]), .din(din), .dout(dout_bank[3]));

    // mux the output of the selected bank
    assign dout = dout_bank[addr[11:10]];

endmodule
