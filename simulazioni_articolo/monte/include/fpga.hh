#include <vector>
#include <iostream>
#include <utility>

// State Machine states
#define s_CYCLE 1
#define s_WRITE 2

class fpga
{
private:
  int state = s_CYCLE;
  int fpga_index = -1;
  int SiPM_number = -1; // this is the SiPM that is being read 0-19
  int number_of_planes = 0;
  int fingers_per_plane = 0;
  std::vector<std::pair<int,int>> channels_i_see{20,std::make_pair(-1,-1)};

public:
  fpga(int index, int planes, int f_per_planes);
  void posedge_clk();
  std::pair<int,int> getSiPMnumber() const;
  int getSiPMindex() const;
  int getFPGAindex() const;
  void which_ch_do_u_see();
};
