#include <vector>
#include <iostream>

#include "calorimeter.hh"


  calorimeter::calorimeter(int depth, int width)
  {
    number_of_planes = depth;
    fingers_per_plane = width;
    number_of_fpga = number_of_planes*fingers_per_plane/20;

    for(int i=0; i<number_of_planes; i++)
    {
      calo_occupancy.push_back(std::vector<int>(fingers_per_plane,0));
      latches_state.push_back(std::vector<int>(fingers_per_plane,0));
    }

    std::cout << "Calorimeter created: [" << number_of_planes << " x " << fingers_per_plane << "] = [ "
              << number_of_planes*0.5 << " cm x ("
              << fingers_per_plane << " x " << fingers_per_plane << ") cm^2 ]" << std::endl;

    fpga_v = std::vector<fpga>(number_of_planes*fingers_per_plane/20);
  };

  void calorimeter::show_occupancy()
  {
    int i=0;
    for (auto p: calo_occupancy)
    {
      std::cout << i;
      if(i<10) std::cout << " ";
      if(i%2==0) std::cout << " X [ ";
      if(i%2==1) std::cout << " Y [ ";

      for(auto ch: p)
      {
        if(ch!=0) std::cout << "\033[91m" ;
        std::cout << ch << " " ;
        std::cout << "\033[0m " ;
      }
      if(i%2==1) std::cout << "]"<< std::endl;
      if(i%2==0) std::cout << "]  ";
      i++;
    }
  };

  void calorimeter::reset_all()
  {
    for (auto& p: calo_occupancy)
    {
      for(auto& ch: p)
      {
        ch *= 0;
      }
    }
  };

  void calorimeter::add_particle(event_dictionary particle)
  {
    float lowest_threshold = 1.25; // MeV
    auto energy = particle.GetEnergy();
    auto level2ID = particle.GetLevel2ID();
    auto level3ID = particle.GetLevel3ID();

    std::cout << "  Singles: " << particle.getNumberOfSingles() << std::endl;
    // Load all signals over threshold in the calorimeter
    for(int i=0; i<particle.getNumberOfSingles(); i++)
    {
//      std::cout << "    " << level2ID[i] << " " << level3ID[i] << std::endl;
      // if(energy[i]<=lowest_threshold) continue;
      calo_occupancy[level2ID[i]][level3ID[i]]++;
    }
  };

  const std::vector<std::vector<int>> calorimeter::getOccupancy() const
  {

    return calo_occupancy;
  };

  void calorimeter::clock_the_calorimeter()
  {
    // here we update the latches states

  };
