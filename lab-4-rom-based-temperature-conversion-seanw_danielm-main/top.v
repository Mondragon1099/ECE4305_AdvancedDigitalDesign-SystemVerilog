`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/16/2026 04:19:58 PM
// Design Name: 
// Module Name: top
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


module top(
    input logic [8:0] SW,
    input logic CLK100MHZ,
    input logic CPU_RESETN,
    output logic [6:0] Cnode,
    output logic [7:0] AN,
    output logic DP
    
    );

logic [7:0] amt; // 8 bit input
logic msb;       // MSB input, decides Binary range splitting and whether display is F or C
logic [11:0] bcd_RegularInput;  // 3 digits * 4 bits = 12 bits
logic [11:0] bcd_ConvInput;
logic [7:0] dataOut;

assign msb = SW[8];
assign amt = SW[7:0];
  
  
synch_rom (
    .clk(CLK100MHZ),
    .addr(SW[8:0]),
    .data(dataOut)
);  

binary_to_BCD #(.INPUT_WIDTH(8), .DECIMAL_DIGITS(3)) regularInput (
    .binary(amt),
    .bcd(bcd_RegularInput)
);

binary_to_BCD #(.INPUT_WIDTH(8), .DECIMAL_DIGITS(3)) convertedOutput (
    .binary(dataOut),
    .bcd(bcd_ConvInput)
);

// Create custom display data with C and F letters 
logic [31:0] display_data;

always_comb begin
    if (msb == 0) begin
        // MSB = 0: Show Celsius (C) and Fahrenheit (F)
        display_data = {
            4'b1111,           // Digit 7 (blank)
            bcd_ConvInput,     // Digits 6,5,4 (converted output)
            4'b1100,           // Digit 3 (C for Celsius)
            bcd_RegularInput   // Digits 2,1,0 (regular input)
        };
    end else begin
        // MSB = 1: Show Fahrenheit (F) and Celsius (C) - swap positions
        display_data = {
            4'b1100,           // Digit 7 (C for Celsius)
            bcd_ConvInput,     // Digits 6,5,4
            4'b1111,           // Digit 3 (F for Fahrenheit)
            bcd_RegularInput   // Digits 2,1,0
        };
    end
end


// Connect to 7-segment driver
seg7_driver display (
    .clk(CLK100MHZ),
    .rst_n(CPU_RESETN),
    .bits(display_data),  // bcd_RegularInput [11:8]=hundreds, [7:4]=tens, [3:0]=ones
    .Cnode(Cnode),
    .dp(DP),
    .AN(AN)
);


endmodule
