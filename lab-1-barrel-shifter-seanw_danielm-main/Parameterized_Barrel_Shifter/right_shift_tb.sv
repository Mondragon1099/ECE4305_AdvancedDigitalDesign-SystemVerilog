`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/26/2026 01:52:05 PM
// Design Name: 
// Module Name: right_shift_tb
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


module right_shift_tb ( 

);

// declare local logic identifiers
parameter N = 4;
    logic [(2**N) - 1:0] a; //input [7:0]
    logic [N - 1:0] amt; // shift amount [2:0]
    logic [(2**N) - 1:0] y; //output
    
// instantiate the module under test
param_right_shifter #(.N(N)) dut ( // dut means device under test
    .a(a), .amt(amt), .y(y)
     );


// specify a stopwatch to stop the simulation
    initial 
    begin
        #40 $finish;
    end

// generate stimula, using initial and always
    initial
    begin
        a = 16'b0000000000000000;
        amt = 4'b0000;
        #10
        a = 16'b1100000000100001;
        amt = 4'b0001;
    end   
// display the output response (text or graphics)


endmodule
