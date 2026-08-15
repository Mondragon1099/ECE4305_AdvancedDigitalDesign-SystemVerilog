`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/27/2026 08:40:12 PM
// Design Name: 
// Module Name: multi_barrel_shifter_reverser
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


module multi_barrel_shifter_reverser #(parameter N = 2)(
    input  logic [15:0] SW,
    output logic [15:0] LED
    );
    logic [N-1:0] amt;
    logic sel;
    logic [(2**N)-1:0] dataoutL, datain;
    logic [(2**N)-1:0] dataoutR;
    logic [(2**N)-1:0] result;
    
    assign amt = SW[15:12];
    assign sel = SW[11];
    assign datain = SW[(2**N)-1:0];  // SW[7:0] when N=3
    
    // Instantiate modules
    //param_left_shifter #(.N(N)) left_shifter (
     //   .datain(datain), 
      //  .amt(amt), 
      //  .dataout(dataoutL)
   // );
   reverser #(.N(N)) left_shifter (
        .in(datain), 
        .amt(amt), 
        .out(dataoutL)
    );
   
    
    param_right_shifter #(.N(N)) right_shifter (
        .a(datain), 
        .amt(amt), 
        .y(dataoutR)
    );
    
    mux2x1 #(.N(N)) mux (
        .a(dataoutL), 
        .b(dataoutR), 
        .sel(sel), 
        .out(result)
    );
    
    // Drive outputs
    assign LED[7:0] = result;
    assign LED[15:8] = 8'b0;  // Drive unused LEDs to 0
    
endmodule
