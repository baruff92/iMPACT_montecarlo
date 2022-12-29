#include <vector>
#include <iostream>

#include "fpga.hh"

fpga::fpga(int index)
{
  fpga_index = index;
  SiPM_number = 0;
  std::cout << "FPGA #" << fpga_index << " created" << std::endl;
}

void fpga::posedge_clk()
{
  // std::cout << "reading " << SiPM_number << " ";
  (SiPM_number = SiPM_number+1);
  // std::cout << SiPM_number << std::endl;
}

int fpga::getSiPMnumber()
{
  return SiPM_number;
}

int fpga::getFPGAindex()
{
  return fpga_index;
}
