`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/23/2026 02:44:35 PM
// Design Name: 
// Module Name: mem_bank
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


module mem_bank(
    input  logic        clk,
    input  logic        we,
    input  logic [9:0]  addr,
    input  logic [3:0]  din,
    output logic [3:0]  dout
);
    logic [3:0] mem [0:1023];

    always_ff @(posedge clk) begin
        if (we)
            mem[addr] <= din;
        dout <= mem[addr];
    end
endmodule
