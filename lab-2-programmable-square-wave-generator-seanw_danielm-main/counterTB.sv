`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/03/2026 07:12:43 PM
// Design Name: 
// Module Name: counterTB
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


module counterTB(

);

// declare local logic identifiers
    logic [3:0] a, b; //input [7:0]
    logic  clk, reset;
    logic  y; //output
    
// instantiate the module under test
    squareWaveGen dut ( // dut means device under test
    .M(a), .N(b), .clk(clk), .out_put(y), .reset(reset)
     );


// Generate clock in separate initial block
    initial begin
        clk = 0;
        forever #5 clk = ~clk;  // 10ns period (100MHz)
    end
    
    // Generate stimulus
    initial begin
        // Initialize signals
        reset = 0;
        a = 4'b0000;
        b = 4'b0000;
        
        #10
        reset = 1;
        
        @(posedge clk);
        a = 4'b0001;
        b = 4'b0010;
        
        // Wait to observe behavior
        #300;
        
        @(posedge clk);
        a = 4'b0010;
        b = 4'b0010;
        
        
        #3000;
        @(posedge clk);
        a = 4'b0011;
        b = 4'b0001;
        
        #3000;
        $finish;
    end
    
// display the output response (text or graphics)


endmodule
