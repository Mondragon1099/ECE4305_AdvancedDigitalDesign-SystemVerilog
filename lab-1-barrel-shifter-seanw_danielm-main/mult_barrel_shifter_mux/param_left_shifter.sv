`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/26/2026 03:32:18 PM
// Design Name: 
// Module Name: param_left_shifter
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


module param_left_shifter #(parameter N = 3)(
    input logic [(2**N)-1:0] datain, // 7:0
    input logic [N-1:0] amt, // 3:0
    output logic [(2**N)-1:0] dataout // 7:0
    );
    
    logic [2**N-1:0][(2**N)-1:0]memory;  // [7:0][7:0], 8 rows, 8 bit width array
    assign memory[0] = datain; // assign first row to the input
    generate
        genvar i;
        for (i=1; i < (2**N); i=i+1) begin
            if (i==1)
                assign memory[i] = {datain << i | datain[2**N-1]}; 
            else
                assign memory[i] = {datain << i | datain[2**N-1:2**N-i]};        
            end
    endgenerate
    
    assign dataout = memory[amt];
    
    endmodule
