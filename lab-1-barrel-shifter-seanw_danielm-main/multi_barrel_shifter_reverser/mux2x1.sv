`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/27/2026 05:48:26 PM
// Design Name: 
// Module Name: mux2x1
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


module mux2x1 #(parameter N = 3) (
    input logic sel,
    input logic [2**N-1:0]a,
    input logic [2**N-1:0]b,
    output logic [2**N-1:0]out
    );
    
    //always_comb begin
    //if (sel)
      //  assign out = a;
    //else
      //  assign out = b;        
   // end
    
     assign out = sel ? a : b; // same as above 
    
endmodule
