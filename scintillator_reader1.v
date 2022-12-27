`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/25/2020 05:40:10 PM
// Design Name: 
// Module Name: scintillator_reader1
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


module scintillator_reader1
#(TIMESTAMP_LEN = 40,
  SIPM_NUMBER = 20
)
(

  // INTERFACE with SiPM_reader1
  // input from comparators
  input [(3 * SIPM_NUMBER) - 1 :0] comparators_input,

  // input from counter
  input [TIMESTAMP_LEN-1:0] counter_input,

  // clock input
  input clk,

  // INTERFACE with latch_reader1
  input rst,
  
  // Interface with SiPM_reader1
  // Interface with FIFO to save data
  input full,

  output [15:0] data_out,
  output wr_en,
  
  // Input for valid reading
  input start_daq
  

    );
    
    
    // Internal wires
    wire [SIPM_NUMBER - 1 : 0] w_clear_latches;
    wire [SIPM_NUMBER  * TIMESTAMP_LEN - 1 : 0] w_timestamp;
    wire [SIPM_NUMBER  * 2 - 1 : 0] w_signal_height;
    
    // Instatiation of the only latch_reader1
    Latch_reader1 latchreader(
    .clk(clk),
    .rst(rst),
    .timestamp_input(w_timestamp),
    .signal_height_input(w_signal_height),
    .clear_SiPM(w_clear_latches),
    .full(full),
    .wr_en(wr_en),
    .data_out(data_out),
    .start_daq(start_daq)
   );
    
    genvar i;
    generate
     for(i = 0; i < SIPM_NUMBER; i = i + 1)
      begin
        SiPM_reader1 sipmreader(
        // inputs
        .comparators_input(comparators_input[(3*i) + 2 : 3*i]),
        .counter_input(counter_input),
        .clk(clk),
        .clear_latches(w_clear_latches[i]),
        // outputs
        .timestamp_output(w_timestamp [(TIMESTAMP_LEN * i) + TIMESTAMP_LEN - 1 : TIMESTAMP_LEN * i]),
        .signal_height_output(w_signal_height[(2*i) + 1 : 2*i])
        );
      end
     endgenerate
    
endmodule
