`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/16/2026 02:04:23 PM
// Design Name: 
// Module Name: synch_rom
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


module synch_rom(
    input logic clk,
    input logic [8:0] addr,  // address in 9 bits (0 - 511)
    output logic [7:0] data // data out, 8 bits (0 - 255)
    );
    
    // signal declaration
    (*rom_style = "block" *)logic [8:0] rom [0:511]; // 512-word x 9 bit array
    
    initial
        $readmemb("C2F_or_F2C.mem", rom); // initilize rom with the memory contents of C2F memory file
        
    always_ff @(posedge clk)
        data <= rom[addr];
endmodule
