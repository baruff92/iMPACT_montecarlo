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

    std::cout << "To build this calorimeter we need " << number_of_fpga << " FPGAs" << std::endl;
    for(int i=0; i<number_of_fpga; i++)
    {
      fpga t_fpga(i, number_of_planes, fingers_per_plane);
      fpga_v.push_back(t_fpga);
      fpga_v[i].which_ch_do_u_see();
    }

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

  void calorimeter::show_latches_state()
  {
    int i=0;
    for (auto p: latches_state)
    {
      std::cout << i;
      if(i<10) std::cout << " ";
      if(i%2==0) std::cout << " X [ ";
      if(i%2==1) std::cout << " Y [ ";

      for(auto ch: p)
      {
        if(ch!=0) std::cout << "\033[92m" ;
        if(ch==-1) std::cout << "x " ;
        else std::cout << ch << " " ;
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

  int calorimeter::add_particle(event_dictionary particle)
  {
    float lowest_threshold = 1.25; // MeV
    auto energy = particle.GetEnergy();
    auto level2ID = particle.GetLevel2ID();
    auto level3ID = particle.GetLevel3ID();
    int hits_added = 0;

    std::cout << "  Singles: " << particle.getNumberOfSingles() << std::endl;
    // Load all signals over threshold in the calorimeter
    for(int i=0; i<particle.getNumberOfSingles(); i++)
    {
//      std::cout << "    " << level2ID[i] << " " << level3ID[i] << std::endl;
      if(energy[i]<=lowest_threshold) continue;
      calo_occupancy[level2ID[i]][level3ID[i]]++;
      hits_added++;
    }
    return hits_added;
  };

  const std::vector<std::vector<int>> calorimeter::getOccupancy() const
  {

    return calo_occupancy;
  };

  void calorimeter::clock_the_calorimeter()
  {
    timestamp_counter++;

    // here we clock the FPGA i.e. the State Machines
    std::cout << "FPGA are reading SiPM:" << std::endl;
    for(auto& f: fpga_v)
    {
      f.posedge_clk();
      std::cout << "#" << f.getFPGAindex() << ":" << f.getSiPMindex() << "(" << f.getSiPMnumber().first << "," << f.getSiPMnumber().second << ") ";
    }
    std::cout << std::endl;

    // here we update the latches states
    // (after the FPGAs, changes will be detected the next clock posedge)
    for(int pl=0; pl<number_of_planes; pl++)
    {
      for(int fn=0; fn<fingers_per_plane; fn++)
      {
        if(latches_state[pl][fn]==-1)
        {
          latches_state[pl][fn]=1;
        }
        if(calo_occupancy[pl][fn]>0 && latches_state[pl][fn]==0)
        {
          latches_state[pl][fn]=-1;
        }
      }
    }

  };
