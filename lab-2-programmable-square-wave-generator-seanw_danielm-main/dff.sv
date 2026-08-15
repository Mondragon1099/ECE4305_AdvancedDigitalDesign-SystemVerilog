`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/03/2026 05:54:03 PM
// Design Name: 
// Module Name: dff
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


module dff #(parameter WIDTH = 8)(
    input logic clk,
    input logic reset,
    input logic [WIDTH:0] d,
    output logic [WIDTH:0] q
);
    always_ff @(posedge clk)
    begin
        if(!reset) // resetN on the Nexys A7 is active low
            q <= 0;
        else
            q <= d;
    end
endmodule
