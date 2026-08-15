`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/25/2026 12:38:41 AM
// Design Name: 
// Module Name: fifo_tb
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


module fifo_tb();

    // signal declarations
    localparam DATA_WIDTH = 16;
    localparam ADDR_WIDTH = 4;
    localparam T = 10; // clock period
    
    logic clk, reset;
    logic wr, rd;
    logic [DATA_WIDTH - 1 : 0]     w_data;
    logic [(DATA_WIDTH/2) - 1 : 0] r_data;
    logic full, empty;
    
    // instantiate module under test
    fifo #(.DATA_WIDTH(DATA_WIDTH), .ADDR_WIDTH(ADDR_WIDTH)) uut (.*);
    
    // 10 ns clock running forever
    always begin
        clk = 1'b1; #(T/2);
        clk = 1'b0; #(T/2);
    end
    
    // reset for the first half cycle
    initial begin
        reset = 1'b1;
        rd    = 1'b0;
        wr    = 1'b0;
        @(negedge clk);
        reset = 1'b0;
    end
    
    // test vectors
    initial begin
        // ----------------EMPTY-----------------------
        @(negedge clk);
        w_data = 16'hABCD; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'h1234; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'hDEAD; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        // read (should get upper 8 bits of 16'hABCD -> 8'hAB)
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk); rd = 1'b0;

        // read (should get lower 8 bits of 16'hABCD -> 8'hCD)
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk); rd = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'hBEEF; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'hCAFE; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'h5A5A; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'hF0F0; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'h0FF0; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'h1357; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'h2468; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'hA5A5; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'h9999; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'h6666; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'h3333; wr = 1'b1;
        @(negedge clk); wr = 1'b0;
        
        repeat(1) @(negedge clk);
        w_data = 16'hAAAA; wr = 1'b1;
        @(negedge clk); wr = 1'b0;
        
        repeat(1) @(negedge clk);
        w_data = 16'hBBBB; wr = 1'b1;
        @(negedge clk); wr = 1'b0;
        
        repeat(1) @(negedge clk);
        w_data = 16'hCCCC; wr = 1'b1;
        @(negedge clk); wr = 1'b0;
        
        repeat(1) @(negedge clk);
        w_data = 16'hDDDD; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        // ----------------FULL-----------------------
        // drain with reads (18 writes x 2 reads each, minus 2 already done = 34 reads to empty)
        repeat(34) begin
            @(negedge clk);
            rd = 1'b1;
            @(negedge clk); rd = 1'b0;
        end

        // ----------------EMPTY-----------------------
        // read and write simultaneously while empty
        repeat(1) @(negedge clk);
        w_data = 16'hAAAA; wr = 1'b1; rd = 1'b1;
        @(negedge clk); wr = 1'b0; rd = 1'b0;
        

        // read while empty (should be ignored)
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk); rd = 1'b0;

        // ----------------NOT EMPTY-----------------------
        repeat(1) @(negedge clk);
        w_data = 16'h1111; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'h2222; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        repeat(1) @(negedge clk);
        w_data = 16'h3333; wr = 1'b1;
        @(negedge clk); wr = 1'b0;

        // read and write simultaneously while not empty
        repeat(1) @(negedge clk);
        w_data = 16'h4444; wr = 1'b1; rd = 1'b1;
        @(negedge clk); wr = 1'b0; rd = 1'b0;

        repeat(3) @(negedge clk);
        $stop;
    end

endmodule
