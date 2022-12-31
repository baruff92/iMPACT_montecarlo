#include <vector>
#include <iostream>

#include "fpga.hh"

fpga::fpga(int index, int planes, int f_per_planes)
{
  fpga_index = index;
  number_of_planes = planes;
  fingers_per_plane = f_per_planes;
  SiPM_number = -1;
  std::cout << "FPGA #" << fpga_index << " created" << std::endl; // - calo layout: "
          //  << number_of_planes << " planes x " << fingers_per_plane << "s ";// << std::endl;

  // here we have to calculate which SiPM channels we can see
  int fpga_per_plane = fingers_per_plane/2; // =4 in a 8x8, =8 in a 16x16
  int x_or_y = (int)(fpga_index/(fpga_per_plane));
  int first_plane = (int)(x_or_y/2)*20+(x_or_y%2);
  // std::cout << x_or_y << " from: " << first_plane << std::endl;

  // fill the channels_i_see vector
  for (int i=0; i<20; i++)
  {
    channels_i_see[i].first = first_plane+2*(i%10);
    channels_i_see[i].second = (int)(i/10) + 2*(fpga_index%fpga_per_plane);
  }
}

void fpga::posedge_clk()
{
  // std::cout << "reading " << SiPM_number << " ";
  SiPM_number++%20;
  // std::cout << SiPM_number << std::endl;
}

int fpga::getSiPMnumber() const
{
  return SiPM_number;
}

int fpga::getFPGAindex() const
{
  return fpga_index;
}

void fpga::which_ch_do_u_see()
{
  std::cout << "FPGA #" << fpga_index << " reads:" << std::endl;
  for(auto c: channels_i_see)
  {
    std::cout << "(" << c.first << "," << c.second << ") ";
  }
  std::cout << std::endl;
}
