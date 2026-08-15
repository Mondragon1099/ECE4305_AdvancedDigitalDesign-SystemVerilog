`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/16/2026 04:10:38 PM
// Design Name: 
// Module Name: binary_to_BCD
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


module binary_to_BCD #(
    parameter INPUT_WIDTH = 8,                           // Input binary width
    parameter DECIMAL_DIGITS = 3                          // Number of decimal digits needed
)(
    input  logic [INPUT_WIDTH-1:0] binary,                // Binary input
    output logic [DECIMAL_DIGITS*4-1:0] bcd               // BCD output (packed)
);

    integer i, j;
    logic [DECIMAL_DIGITS*4-1:0] bcd_temp;
    
    always_comb begin
        bcd_temp = '0;  // Initialize to zero
        
        // Double-dabble algorithm
        for (i = INPUT_WIDTH-1; i >= 0; i--) begin
            // Add 3 to any BCD digit >= 5
            for (j = 0; j < DECIMAL_DIGITS; j++) begin
                if (bcd_temp[j*4 +: 4] >= 5)
                    bcd_temp[j*4 +: 4] = bcd_temp[j*4 +: 4] + 3;
            end
            
            // Shift left and bring in next binary bit
            bcd_temp = {bcd_temp[DECIMAL_DIGITS*4-2:0], binary[i]};
        end
        
        bcd = bcd_temp;
    end

endmodule
