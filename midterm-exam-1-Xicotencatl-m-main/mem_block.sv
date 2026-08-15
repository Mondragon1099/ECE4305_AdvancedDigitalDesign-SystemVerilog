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
    input logic clk, 
    input logic we, //write enable
    input logic [11:0] addr, // 12 bit data input 2^12 = 4096 = 0 - 4095
    input logic [3:0] din,  // 4 bit data input, 2^4 = 16 0-15
    output logic [3:0] dout // 4 bit data output
    );
    
    // TODO: Write your code here 
    // DO NOT CHANGE THE MODULE INTERFACE     
        
    // 4 banks of 1024 x 4 = 4096 x 4 total
    logic [3:0] memory [0:3][0:1023];

    // Upper 2 bits select bank, lower 10 bits select word
    logic [1:0]  bank = addr[11:10];
    logic [9:0]  offset = addr[9:0];

    always_ff @(posedge clk) begin
    if (we)
        // write
        memory[bank][offset] <= din;
        
        // read
        dout <= memory[bank][offset]; 
    
    end
    
endmodule
