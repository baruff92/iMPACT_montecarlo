`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: UNIPD and INFN-PD
// Physicist: Baruffaldi e Chiappara
// 
// Create Date: 19.11.2020 17:15:00
// Design Name: 
// Module Name: SiPM_reader
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

// TODO
// Sometimes a signal arriving during a reset cycle gets the wrong timestamp.
// rightnow we do not issue this problem, we will (eventually) fix it in the future

module SiPM_reader1#(TIMESTAMP_LEN = 40)(

    // input from comparators
    input [2:0] comparators_input,
    
    // input from counter
    input [TIMESTAMP_LEN-1:0] counter_input,
    
    // clock input
    input clk,
    
    // clear the latches and timestamp values
    input clear_latches,
    
    // time stamp output
    output [TIMESTAMP_LEN-1:0] timestamp_output,
    
    // value of the signal translated from comparators
    output [1:0] signal_height_output
    );

    // support internal register for timestamp
    reg [TIMESTAMP_LEN-1:0] r_timestamp_output = 0;
    
    // register to wait a clock cycle before reading signal height.
    // It looks fine for cosmic, will this be ok with protons?
    // TODO
    reg listening = 0 ;
    
    // support internal register for signal height output
    reg [1:0] r_signal_height_output = 0;
    
    // support wire for latching purposes
    wire [2:0] wQbar;
    
    // support wire used as latch output and as input
    // for signal height
    wire [2:0] latch_intraput;
    reg [2:0] r_latch_intraput = 0;
   
   assign latch_intraput = r_latch_intraput;
   
   
   // test to solve issues about zeros and low numbers
    wire [40:0] tst_intraput;
    reg [40:0] r_tst_intraput = 0;
   
   assign tst_intraput = r_tst_intraput;
   
    // input for latches used to deny undetermined state during reset
    wire [2:0] gate_comparators_input;
    wire [40:0] gate_timestamp_input;
    
    // Check between gate input and comparator input to deny undetermined state
    assign gate_comparators_input = clear_latches == 1'b1 ? 0 : comparators_input;
    assign gate_timestamp_input = clear_latches == 1'b1 ? 0 : counter_input;

    // assignment of wires to their register
    assign signal_height_output = r_signal_height_output ;
    assign timestamp_output = r_timestamp_output ;
           
   // Latches for each comparator of a single SiPM.
//    nor gate1_0 (latch_intraput[0], clear_latches, wQbar[0]);
//    nor gate2_0 (wQbar[0], gate_comparators_input[0], latch_intraput[0]); 
        
//    nor gate1_1 (latch_intraput[1], clear_latches, wQbar[1]);
//    nor gate2_1 (wQbar[1], gate_comparators_input[1], latch_intraput[1]); 

//    nor gate1_2 (latch_intraput[2], clear_latches, wQbar[2]);
//    nor gate2_2 (wQbar[2], gate_comparators_input[2], latch_intraput[2]); 
    
    always @ (clear_latches, gate_comparators_input )
        begin
            if(clear_latches)
                begin
                  r_latch_intraput <= 3'b000;
                  r_tst_intraput <= 40'b1111111111111111111111111111111111111111;
                end
            else if(gate_comparators_input > r_latch_intraput)
                begin
                    r_latch_intraput <= gate_comparators_input ;
                    r_tst_intraput <= counter_input;
                end
            else
                begin
                end
        end
        
        
    
    always @ (posedge clk)
    begin
       //$display("Comparators_input = %b   counter_input = %b    clock = %b  height = %b     clear = %b listening = %b   latch_intraput = %b",
       //          comparators_input,counter_input, clk, signal_height_output, clear_latches, listening, latch_intraput);
       // External reset
       if(clear_latches == 1) 
           begin
                r_timestamp_output <= 40'b1111111111111111111111111111111111111111;
                r_signal_height_output <= 0 ;
                listening <= 0 ;                
            end
                        
       else 
            begin     
                // If a signal is seen, start listening
                if( clear_latches == 0 && latch_intraput != 0  && r_signal_height_output == 2'b00 && listening == 0 )
                    begin
                        listening <= 1 ;
                        //r_timestamp_output <= counter_input;
                    end
                    
                
                // If I am listening, save signal and stop listening
                /*else */if (clear_latches == 0 && listening == 1)
                    begin
                       
                        if (latch_intraput == 3'b001)
                            begin
                                r_signal_height_output <= 2'b01 ;
                            end
                        else if (latch_intraput[1] == 1 && latch_intraput[2] == 0 )
                            begin
                                r_signal_height_output <= 2'b10 ;                    
                            end
                        else if (latch_intraput[2] == 1)
                            begin                        
                                r_signal_height_output <= 2'b11; 
                            end
                        r_timestamp_output <= r_tst_intraput;
                        listening <= 0;
                    end
                    else
                     begin
                     
                     end
              end
    end
    
   
endmodule