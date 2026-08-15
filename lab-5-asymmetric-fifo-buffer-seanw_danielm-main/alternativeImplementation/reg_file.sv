`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/25/2026 12:33:49 AM
// Design Name: 
// Module Name: reg_file
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


module reg_file
    #(parameter ADDR_WIDTH = 3, DATA_WIDTH = 8)
    (
        input logic clk,
        input logic w_en,
        input logic [ADDR_WIDTH : 0] r_addr, // reading address, 4 bits for 16 memory locations
        input logic [ADDR_WIDTH - 1: 0] w_addr, // writing address, 3 bits for 8 memory locations
        input logic [DATA_WIDTH - 1: 0] w_data, // writing 8 bits
        output logic [(DATA_WIDTH / 2) - 1: 0] r_data // reading only 4 bits
    );
    
    // signal declaration
    logic [ (DATA_WIDTH / 2) - 1: 0] memory [0: 2 **(ADDR_WIDTH + 1) - 1]; // 16x4, 16 locations, each 4 bits
    
    // Write operation: split 8-bit write into two 4-bit locations
    always_ff @(posedge clk) begin
        if (w_en) begin
            memory[{w_addr, 1'b0}] <= w_data[DATA_WIDTH - 1 : DATA_WIDTH/2];  // upper half address
            memory[{w_addr, 1'b1}] <= w_data[(DATA_WIDTH/2) - 1 : 0];         // lower half address
        end
    end
            
    // read operation, only 4 bits
    assign r_data = memory[r_addr];
endmodule
