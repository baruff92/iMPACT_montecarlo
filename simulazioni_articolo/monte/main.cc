#include <TStyle.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TF1.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TGraph.h>
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
#include <numeric>
#include <random>
#include <fstream>
#include <memory>
#include <stdint.h>
#include "TApplication.h"
#include "TRootCanvas.h"

// #include "event_dictionary.hh"
#include "calorimeter.hh"

int main()
{
	TApplication app("app", nullptr, nullptr);
  // Starting time for stats
  auto start = std::chrono::system_clock::now();
  std::time_t start_time = std::chrono::system_clock::to_time_t(start);
  std::cout << "Computation started at " << std::ctime(&start_time) << std::endl;

  TFile *f = new TFile("../../simu_paper_1_short_100.root");     		// opens the file .root

  TTree *singles=(TTree*)f->Get("Singles");
  unsigned int single_events = singles->GetEntries();
  std::cout << "Singles = "<< single_events << std::endl;

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
  unsigned int number_of_events_dic = (unsigned int)ev_id_s + 1;

  //  IMPORTANT this is the particle rate to simulate
  float particle_rate = 100000; // Hz
  float number_of_rec_part=20;
  float scan_time = (float)number_of_rec_part/particle_rate; //s
	float fpga_clock_speed = 1e5; //Hz
	double clock_cycle = 1/fpga_clock_speed; // s = 100 KHz

  std::cout << "-----------------------------------------" << std::endl;
  std::cout << "Simulation parameters:" << std::endl;
  std::cout << "Dictionary size: " << number_of_events_dic << std::endl;
  std::cout << "Number of particles to simulate: " << number_of_rec_part << std::endl;
  std::cout << "Target particle rate: " << particle_rate << " Hz" << std::endl;
  std::cout << "FPGA clock spees: " << fpga_clock_speed << " Hz" << std::endl;
  std::cout << "Scan time: " << scan_time << " s" << std::endl;

  //  CALORIMETER parameters
  int number_of_planes = 60;
  int fingers_per_plane = 8;
	double finger_width = 1; // cm
	double finger_depth = 0.5; // cm

  std::cout << "-----------------------------------------" << std::endl;
  std::cout << "Calorimeter configuration:" << std::endl;
  std::cout << "Depth: " << number_of_planes << " planes" << std::endl;
  std::cout << "Width: " << fingers_per_plane << " scintillator per plane" << std::endl;

  calorimeter my_calo(number_of_planes, fingers_per_plane);

  // MASTER canvas
  TCanvas *c1 = new TCanvas("c1","c1",800,800);
	TRootCanvas *rc = (TRootCanvas *)c1->GetCanvasImp();
  rc->Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");

  // PLOTS
  TH1D* events_multiplicity = new TH1D ("events_multiplicity", "Singles in each event;Signles;Counts", 100, 0, 100);
  TH1D* time_dist = new TH1D ("time_dist", "Particle rate;Rate (Hz);Counts", particle_rate*10, 0, particle_rate*2);
  TH1D* timestamps = new TH1D ("timestamp", "Temporal distributionm of timestamps;time (s);Counts", scan_time*1e6, -scan_time*0.1, scan_time*1.1);
  TH1D* rnd_events = new TH1D ("rnd_events", "Dictionary events extracted;event;Counts", 1000, -number_of_events_dic*0.1, number_of_events_dic*1.1);

  std::vector<event_dictionary> dict(number_of_events_dic);

  for (unsigned int jj=0; jj<single_events; jj++)
  {
    singles->GetEntry(jj);

    dict[ev_id_s].add_event((float)en_s,
                     (unsigned int)lev2_id_s,
                     (unsigned int)lev3_id_s);

     // energy_histo->Fill((float)en_s);

    // std::cout << "ev_id_s:" << ev_id_s << " tim_s:" << tim_s << " lev2_id_s:" << lev2_id_s << " lev3_id_s:" << lev3_id_s << std::endl;
  }

  int jj = 0;
  for (auto ev: dict)
  {
    // std::cout << "dict[" << jj <<"].size(): " << ev.getNumberOfSingles() << std::endl;
    events_multiplicity->Fill(ev.getNumberOfSingles());
    jj++;
  }

  events_multiplicity->Draw();

  // Draw random times uniformly distributed
  double exp_dist_mean   = 10;
  // double exp_dist_lambda = 1 / exp_dist_mean;
  double exp_dist_lambda = particle_rate;
  std::srand(time(NULL));
  std::random_device rd;
  std::seed_seq seed2{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
  std::exponential_distribution<> rng (exp_dist_lambda);
  std::mt19937 rnd_gen_exp (seed2);

  std::uniform_int_distribution<int> uniform_dist(0, number_of_events_dic-1);
  std::mt19937 rnd_gen_unif (seed2);

 /*  for(int i=0; i<0; i++)
  {
    double tstamp=0;
    int count=0;
    while (tstamp < scan_time)
    {
      tstamp+=rng(rnd_gen);
      count++;
    }
    std::cout << i << std::endl;
    time_dist->Fill(count/scan_time);
  }*/

//  my_calo.show_occupancy();

	// canvas for visualisement of events
  TCanvas *c_update = new TCanvas("c_update","c_update",800,800);

  for(int i=0; i<1; i++)
  {
    double tstamp = 0;
    int count = 0;
    std::vector<double> timestamp_vec;
    std::vector<double>  events_from_dic_vec;
    while (tstamp < scan_time)
    {
      tstamp+=rng(rnd_gen_exp);
      count++;
      timestamps->Fill(tstamp);
      timestamp_vec.push_back(tstamp);
      int rnd_event_from_dic = uniform_dist(rnd_gen_unif);
      rnd_events->Fill(rnd_event_from_dic);
      events_from_dic_vec.push_back(rnd_event_from_dic);
    }
    std::cout << "Simulated number of events: " << count << std::endl;

    time_dist->Fill(count/scan_time);
    if(timestamp_vec.size() != events_from_dic_vec.size())  std::cout << "Length of random vectors: " << timestamp_vec.size() << " " << events_from_dic_vec.size() << std::endl;

		// loop over 10 ns clock cycles
		double lab_time = 0;
		unsigned int number_of_clock_cycles = 0;
		unsigned int particle_index= 0;

		while (lab_time<=timestamp_vec.back()+3*clock_cycle)
		{

			std::cout << "Cycle: " << number_of_clock_cycles << " : " << lab_time << " s" << std::endl;
			// Search if a particle arrived in the previous clock cycle
			for(unsigned int j=particle_index; j<timestamp_vec.size(); j++)
			{
				if (timestamp_vec[j]<lab_time)
				{
					std::cout << "	I found at: " << timestamp_vec[j] << " s particle # " << events_from_dic_vec[j] << " j: " << j << std::endl;
					particle_index = j+1;
					int hits_added = my_calo.add_particle(dict[events_from_dic_vec[j]]);
					my_calo.show_occupancy();
					// my_calo.reset_all();
				}

				if (timestamp_vec[j]>=lab_time)
				{
					// std::cout << "		break j: " << j << std::endl;
					break;
				}
			}
			my_calo.clock_the_calorimeter();
			my_calo.show_latches_state();

			// std::cout << "	particle_index: " << particle_index << std::endl;
			number_of_clock_cycles++;
			lab_time += clock_cycle;

		}

		std::cout << "Total number of clock cycles: " << number_of_clock_cycles << " Last timestamp: " << timestamp_vec.back() << std::endl;

  }

// TODO: mettere eventi nel calorimetro

  TCanvas *c2 = new TCanvas("c2","c2",800,800);
  time_dist->Draw();

  TCanvas *c3 = new TCanvas("c3","c3",800,800);
  timestamps->Draw();

  TCanvas *c4 = new TCanvas("c4","c4",800,800);
  rnd_events->Draw();

	// Finishing time for stats
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	int elapsed_seconds_int = (int)(elapsed_seconds.count());
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	std::cout << "Finished computation at " << std::ctime(&end_time)
					 << "Elapsed time = " << elapsed_seconds.count() << " s 	(= "
					 << (int)(elapsed_seconds_int/3600) << "h "<< (int)((elapsed_seconds_int % 3600)/60)
					 << "min " <<elapsed_seconds_int % 60 <<"s)" << std::endl;

  app.Run();
  return 0;

}
