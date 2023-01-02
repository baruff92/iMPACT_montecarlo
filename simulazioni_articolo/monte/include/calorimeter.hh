#include <vector>
#include <iostream>
#include <TCanvas.h>

#include "event_dictionary.hh"
#include "fpga.hh"

class calorimeter
{
private:
  //  CALORIMETER parameters
  int number_of_planes = 0;
  int fingers_per_plane = 0;
  int number_of_fpga = 0;
  int timestamp_counter = 0;

  std::vector<std::vector<int>> calo_occupancy; // 0 and +1 for every particle
  std::vector<std::vector<int>> latches_state; // 0 if empty; -1 if listening; +1 if particle; 7 if in reset
  std::vector<fpga> fpga_v;


public:
  calorimeter(int depth, int width);

  void show_occupancy();
  void show_latches_state();
  void reset_all();
  int add_particle(event_dictionary particle);

  const std::vector<std::vector<int>> getOccupancy() const;

  void clock_the_calorimeter();

};
