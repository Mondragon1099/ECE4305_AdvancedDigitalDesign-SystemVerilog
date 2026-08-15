`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/25/2026 09:04:01 PM
// Design Name: 
// Module Name: param_right_shifter
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


module param_right_shifter #(parameter N = 3)(

    input logic [(2**N) - 1:0] a, //input [7:0]
    input logic [N - 1:0] amt, // shift amount [2:0]
    output logic [(2**N) - 1:0] y //output
    );
    
   // logic [7:0] s0, s1;
   
   //constant declaration
   localparam N1 = (2**N) - 1; // this is 7 from our parameter value = 3
   // signal delcaration
   logic [N:0][N1:0] s; // 3 memory rows, of 8 bits data width
   assign s[0] = amt[0] ? {a[0], a[N1:1]} : a;
   
   generate 
    genvar i;
        for(i = 1; i < N; i = i+1)
            assign s[i] = amt[i] ? { s[i-1][(2**i)-1:0], s[i-1][N1:2**i] } : s[i-1] ;
        
    endgenerate
    
    assign y = s[N-1];
    
   // assign s0 = amt[0] ? {a[0], a[7:1]} : a;
   // assign s1 = amt[1] ? {s0[1:0], s0[7:2]} : s0;
   // assign y = amt[2] ? {s1[3:0], s1[7:4]} : s1;
    
endmodule
