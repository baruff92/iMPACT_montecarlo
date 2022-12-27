`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/24/2020 06:01:57 PM
// Design Name: 
// Module Name: Latch_reader1
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


module Latch_reader1
#(TIMESTAMP_LEN = 40,
  SIPM_NUMBER = 20
)
(

  input clk,
  input rst,
  
  // Interface with SiPM_reader1
  input [(SIPM_NUMBER * TIMESTAMP_LEN) -1 : 0] timestamp_input,
  input [(SIPM_NUMBER * 2) -1 : 0] signal_height_input,
  output [SIPM_NUMBER-1 : 0] clear_SiPM, 
  
  // Interface with FIFO to save data
  input full,
  output wr_en,
  output [15:0] data_out,
  
  // Input for valid reading
  input start_daq
  
    );
    
  localparam state_STOP = 3'b111;
  localparam state_CYCLE = 3'b001;
  localparam state_WRITE = 3'b011;
  localparam state_RESET = 3'b010;
  localparam state_RESETALL = 3'b100;
  localparam state_ERROR = 3'b000;
  localparam state_STEADY = 3'b101;
  
  reg[2:0] state = state_STOP;
  
  reg [SIPM_NUMBER-1 : 0] r_clear_SiPM = 0;
  assign clear_SiPM = r_clear_SiPM;
  
  reg r_wr_en = 0;
//  assign wr_en = full? 0 : r_wr_en;
    assign wr_en = r_wr_en;

  reg [15:0] r_data_out = 0;
  assign data_out = r_data_out;
  
  
  reg [4:0] SiPM_number = 0;
  

  
  reg writing_cycle_number = 0;
  
  reg [47:0] data_buf = 0;

  
  
  always @(posedge clk)
   begin
    case (state)
    
     // STOP waits for a start_daq. If it arrives, goes to cycle,
     // if it is equal to zero, stays in STOP
     state_STOP :
      begin
      
       // Setup of registers
       r_clear_SiPM          <= 0;
       r_wr_en               <= 0;
       r_data_out            <= 0;
       SiPM_number           <= 0;
       data_buf              <= 0;
       writing_cycle_number  <= 0;
       
       if(start_daq == 1'b1)
        begin
         state <= state_RESETALL;
        end
       else
        begin
         state <= state_STOP;
        end
      end
      
      
     state_CYCLE :
     begin
        $display("SiPM number: %0d @ %0d", SiPM_number, $time);
       // First update the address of SiPM to see if
       // something has data
        if(SiPM_number < SIPM_NUMBER-1)
            begin
             SiPM_number <= SiPM_number + 1;
            end
        else if( SiPM_number == SIPM_NUMBER-1)
            begin
             SiPM_number <= 0;
            end
         
         if(r_clear_SiPM[SiPM_number == 0 ? 19 : SiPM_number - 1] == 1'b1)
           begin
             r_clear_SiPM[SiPM_number == 0 ? 19 : SiPM_number - 1] <= 0;
           end

        // If daq is not enabled, go to resetall
        // and eventually stop
        if(start_daq == 1'b0)
            begin
              r_wr_en <= 1'b0;
              state <= state_RESETALL;
            end
         
        // if there is a signal save data_buf with bits that will
        // be sent to FIFO: parity, address, height and timestamp.
        // Then, starts the first writing cycle
        // and goes to write state. Parity is calculated with
        // a reduction XNOR
        // at the end, set clear signal for SiPM to one.
        else if(signal_height_input[2 * SiPM_number +: 2] != 0 )//&& full != 1'b1)
        begin
            $display("SiPM_number: %d   signal_height_input: %d      total signal_height_input: %b \n ", SiPM_number, signal_height_input[2 * SiPM_number +: 2] , signal_height_input );
            r_clear_SiPM[SiPM_number] <= 1;
         
            if(full != 1'b1)
            begin
               r_wr_en <= 1;
               data_buf[47] <= ~^{SiPM_number, signal_height_input[2 * SiPM_number +: 2],
                                   timestamp_input[TIMESTAMP_LEN * SiPM_number +: TIMESTAMP_LEN]};
               data_buf[46:0] <= {SiPM_number, signal_height_input[2 * SiPM_number +: 2],
                                   timestamp_input[TIMESTAMP_LEN * SiPM_number +: TIMESTAMP_LEN]};
               r_data_out [15] <= ~^{SiPM_number, signal_height_input[2 * SiPM_number +: 2],
                                   timestamp_input[TIMESTAMP_LEN * SiPM_number +: TIMESTAMP_LEN]};
               r_data_out [14:10] <= SiPM_number;
               r_data_out [9:8] <= signal_height_input[2 * SiPM_number +: 2];
               r_data_out [7:0] <= timestamp_input[(TIMESTAMP_LEN * SiPM_number)  + 32 +: 8];
               
               state <= state_WRITE;
            end
            else
            begin
               r_wr_en <= 1'b0;
               state <= state_CYCLE;
            end
        end
             
             // If I have not seen a signal, do not write and stay in CYCLE 
        else
        begin
         r_wr_en <= 1'b0;
         state <= state_CYCLE;
        end
     end
      
      
     state_WRITE :
      
      begin
      
       // Stop the clear signal
       if(SiPM_number == 0)
       begin
         r_clear_SiPM[SIPM_NUMBER - 1] <= 1'b0;
       end
       else
        begin
         r_clear_SiPM[SiPM_number - 1] <= 1'b0;
        end
        
       // Keeps on writing to FIFO
       r_wr_en <= 1;
      
      // This part commented to avoid writing a partial event 
       // If I am not acquiring, go to resetall
 /*      if(start_daq == 1'b0)
       begin
        state <= state_RESETALL;
       end
       
       // If I am on the first writing cycle
       else*/ if(writing_cycle_number == 1'b0)
        begin
         r_data_out <= data_buf[31:16];
         writing_cycle_number <= writing_cycle_number + 1;
         state <= state_WRITE;
        end
        
        // If I am on the second writing cycle,
        // also set register for number of
        // writing cycles to zero.
        else if(writing_cycle_number == 1'b1)
         begin
          r_data_out <= data_buf[15:0];
          writing_cycle_number <= 1'b0;
          state<= state_CYCLE;
         end
         
         // If never, go error
         else
          begin
           state <= state_ERROR;
          end
        
       
      end
      
    // should never happen e probabilmente ti cancellerò
     state_RESET :
      begin
       state <= state_ERROR;
      end
      
      // Sets reset to zero, preparing for cycling
      state_STEADY :
       begin
        if(start_daq == 1'b1)
         begin
          r_clear_SiPM <= 0;
          state <= state_CYCLE;
         end
        else if(start_daq == 1'b0)
         begin
          state <= state_RESETALL;
         end
        end
      
     // Resets all latches and sets to zero writing to FIFO.
     // goes to stop or cycle
     // depending on start_daq value.
     state_RESETALL :
      begin
       r_wr_en               <= 0;
       r_data_out            <= 0;
       SiPM_number           <= 0;
       data_buf              <= 0;
       writing_cycle_number  <= 0;
       // there should be a better way.
       r_clear_SiPM <= ~0;
       
       if(start_daq == 1'b1)
        begin
         state <= state_STEADY;
        end
       else if(start_daq == 1'b0)
        begin
         state <= state_STOP;
        end
      end
      
     // Error, should never happen
     state_ERROR :
      begin
      end
      
     default:
      state <= state_RESETALL;
   endcase
  end
  

endmodule
