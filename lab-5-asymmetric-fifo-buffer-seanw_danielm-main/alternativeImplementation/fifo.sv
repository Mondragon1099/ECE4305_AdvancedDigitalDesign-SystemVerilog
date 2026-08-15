`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/25/2026 12:36:13 AM
// Design Name: 
// Module Name: fifo
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


module fifo
    #(parameter ADDR_WIDTH = 4, DATA_WIDTH = 16)
    (
        input logic clk, reset,
        input logic wr, rd,
        input logic [DATA_WIDTH - 1: 0] w_data, // write 16 bits
        output logic [(DATA_WIDTH / 2) - 1 : 0] r_data, // read 8 bits
        output logic full, empty
    );
    
    // signal declaration
    logic [ADDR_WIDTH - 1: 0] w_addr; // 4 bits for 16 locations
    logic [ADDR_WIDTH : 0] r_addr; // 5 bits for 32 locations
    
    
    // instantiate register file
    reg_file #(.ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(DATA_WIDTH))
        r_file_unit (.w_en( wr & ~full), .*);

    // instantiate fifo controller
    fifo_ctrl #(.ADDR_WIDTH(ADDR_WIDTH))
        ctrl_unit (.*);                    
endmodule
