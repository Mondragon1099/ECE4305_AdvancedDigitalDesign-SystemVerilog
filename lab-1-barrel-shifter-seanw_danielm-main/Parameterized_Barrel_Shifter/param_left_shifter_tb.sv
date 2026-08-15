`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/26/2026 01:14:34 PM
// Design Name: 
// Module Name: param_left_shifter_tb
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


module param_left_shifter_tb #(parameter N = 3) (
   
    );
    
    logic [(2**N)-1:0]datain;
    logic [N:0]amt;
    logic [(2**N)-1:0]dataout;
    
    param_left_shifter dut(.datain(datain), .amt(amt), .dataout(dataout));
    
    integer j = 0; 
    initial begin
    $monitor(datain, amt, dataout);
    datain = 8'b01101100;
    
    for(j=0; j < (2**N); j = j+1) begin
    #100;
    amt = j;
    #100;
    end
    
    end
    
endmodule
