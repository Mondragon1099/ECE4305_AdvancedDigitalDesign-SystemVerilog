`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/27/2026 08:00:12 PM
// Design Name: 
// Module Name: reverser
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


module reverser #(parameter N = 3)(
    input  logic [N**2-1:0] in,
    input logic [N - 1:0] amt, // shift amount [2:0]
    output logic [N**2-1:0] out 
    );
    
    logic [N**2-1:0] s;
    logic [N**2-1:0] d;
    
    always_comb begin
        for (int i = 0; i < N**2; i++) begin
            s[i] = in[(N**2-1)-i];
            end
        end
        
     param_right_shifter #(.N(N)) shifter (
        .a(s), 
        .amt(amt), 
        .y(d)
    );  
     
    always_comb begin
        for (int i = 0; i < N**2; i++) begin
             out[i] = d[(N**2-1)-i];
            end
        end    
        
endmodule
