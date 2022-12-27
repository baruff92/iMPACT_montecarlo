#include <vector>
#include <iostream>

// State Machine states
#define s_CYCLE 1
#define state_WRITE 2

class fpga
{
private:
  int state = s_CYCLE;
  int fpga_index = 0;
  int SiPM_number = 0; // this is the SiPM that is being read 0-19

public:
  void posedge_clk();
  int getSiPMnumber();


};
