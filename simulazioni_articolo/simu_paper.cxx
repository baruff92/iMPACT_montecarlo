// R__LOAD_LIBRARY(/home/gate/cernbox/iMPACT_Filippo/simulazioni_articolo/simu_paper_cxx.so)

#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TGraphErrors.h>
#include <TNtuple.h>
#include <TString.h>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <cmath>
#include <chrono>
#include <time.h>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <random>

// A single event is a signal in a detector volume
// i.e. one particle (unique parentID) produces as many single events
// as the number of detector volumes it passes through
class single_event
{
private:
  std::vector<unsigned int> parentID;
  std::vector<double> og_time;  // timestamp from Geant
  std::vector<float> energy;
  std::vector<unsigned int> level2ID;
  std::vector<unsigned int> level3ID;
  std::vector<double> shifted_time; // timestamp after bootstrap

  // the number of signals
  unsigned int number_of_singles = 0;

  // the number of primary particles
  unsigned int number_of_events = 0;

public:
  void add_event(unsigned int pID, double o_t, float en, unsigned int l2ID, unsigned int l3ID)
  {
    parentID.push_back(pID);
    og_time.push_back(o_t);
    energy.push_back(en);
    level2ID.push_back(l2ID);
    level3ID.push_back(l3ID);
    shifted_time.push_back(o_t);
    number_of_singles++;

    if (pID > number_of_events)
    {
      number_of_events = pID;
    }
  }

  void apply_timeshifts(std::vector<double> time_shifts)
  {
    if(time_shifts.size() != number_of_events+1)
    {
      std::cout << "Wrong number of events!" << std::endl;
      return;
    }

    for (unsigned int i=0; i<number_of_singles; i++)
    {
      shifted_time[i] = og_time[i] + time_shifts[parentID[i]];
      // std::cout << "parentID:" << parentID[i] << " og_time[i]:" << og_time[i] << "  shifted_time[i]:" << shifted_time[i] << std::endl;
    }
  }

  int getNumberOfEvents() {return number_of_events+1;}
  int getNumberOfSingles() {return number_of_singles;}
  float getDepositedEnergy(int i) {return energy[i];}
};

class calo_channel
{
private:
  int plane_number;
  int ch_in_plane;

  int latch;
  int reset_in;

public:

};

//  unit that read 2x10 channels
class FPGA
{
private:
  std::vector<std::string> states{"CYCLE", "WRITE"};

public:
  std::string direction;   // "X", "Y"
  int min_plane;
  int max_plane;
  int min_sub_channel;
  int max_sub_channel;

  std::string FSM_state = "CYCLE";
  int local_channel_scanning = 0; // 0 - 19
};

void simu_paper()
{

  // Starting time for stats
  auto start = std::chrono::system_clock::now();
  std::time_t start_time = std::chrono::system_clock::to_time_t(start);
  cout << "Computation started at " << std::ctime(&start_time) << endl;

  TFile *f = new TFile("./simu_paper_1_short.root");     		// opens the file .root

  TTree *singles=(TTree*)f->Get("Singles");
  unsigned int single_events = singles->GetEntries();
  cout << "Singles = "<< single_events << endl;

  // we dont really need the single hits here
  TTree *hits=(TTree*)f->Get("Hits");
  unsigned int hit_events = hits->GetEntries();
  cout << "Hits = "<< hit_events << endl;

  int ev_id_s;
  double tim_s;
  float en_s;
  int lev2_id_s;
  int lev3_id_s;

  singles->SetBranchAddress("eventID",&ev_id_s);
  singles->SetBranchAddress("time",&tim_s);
  singles->SetBranchAddress("energy",&en_s);
  singles->SetBranchAddress("level2ID",&lev2_id_s);
  singles->SetBranchAddress("level3ID",&lev3_id_s);

  // get the parentID of the last particle to get the number of particles
  singles->GetEntry(single_events-1);
  unsigned int number_of_events = (unsigned int)ev_id_s + 1;

  //  IMPORTANT this is the particle rate to simulate
  float particle_rate = 10; // Hz
  float scan_time = (float)number_of_events/particle_rate; //s
  std::cout << "Number of particles: " << number_of_events << std::endl;
  std::cout << "Target particle rate: " << particle_rate << " Hz" << std::endl;
  std::cout << "Scan time: " << scan_time << " s" << std::endl;

  // Plots
  TH1F* times_histo = new TH1F("Timestamps shifts", "time_hist;time (s);Counts", 100, -scan_time*0.1, scan_time*1.1);
  TH1F* energy_histo = new TH1F("Deposited Energy", "en_hist;Energy (MeV);Counts", 100, -1, 10);

  std::vector<double> time_shifts(number_of_events);

  single_event Events;

  for (unsigned int jj=0; jj<single_events; jj++)
  {
    singles->GetEntry(jj);

    Events.add_event((unsigned int)ev_id_s,
                     (double)tim_s,
                     (float)en_s,
                     (unsigned int)lev2_id_s,
                     (unsigned int)lev3_id_s);

     energy_histo->Fill((float)en_s);

    // std::cout << "ev_id_s:" << ev_id_s << " tim_s:" << tim_s << " lev2_id_s:" << lev2_id_s << " lev3_id_s:" << lev3_id_s << std::endl;
  }

  std::cout << "Number of Singles: " << Events.getNumberOfSingles() << std::endl;
  std::cout << "Number of Particles: " << Events.getNumberOfEvents() << std::endl;

  int number_of_repetitions = 1;

  for (int s=0; s < number_of_repetitions; s++)
  {
    std::cout << s << std::endl;
    // Create a vector with the random timeshifts for each particle
    random_device rnd_device;
    mt19937 mersenne_engine {rnd_device()};
    uniform_int_distribution<int> dist {1, 10000};
    auto gen = [&dist, &mersenne_engine, &scan_time](){return dist(mersenne_engine)/10000.*scan_time;};
    generate(begin(time_shifts), end(time_shifts), gen);

    Events.apply_timeshifts(time_shifts);

    for (auto& it : time_shifts)
    {
      times_histo->Fill(it);
      // std::cout << it << std::endl;
    }
  }

  // Ending time for stats
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  int elapsed_seconds_int = (int)(elapsed_seconds.count());
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);
  cout << "Computation ended at " << std::ctime(&end_time)
       << "Elapsed time = " << elapsed_seconds.count() << " s 	(= "
       << (int)(elapsed_seconds_int/3600) << "h "<< (int)((elapsed_seconds_int % 3600)/60)
       << "min " <<elapsed_seconds_int % 60 <<"s)" << endl;

  // Plots
  TCanvas* c1= new TCanvas("c1","c1");
  times_histo->Draw();

  TCanvas* c2= new TCanvas("c2","c2");
  energy_histo->Draw();
}
