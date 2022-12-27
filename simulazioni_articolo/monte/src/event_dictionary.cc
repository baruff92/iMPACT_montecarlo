#include <vector>
#include <iostream>
#include "event_dictionary.hh"


void event_dictionary::add_event(float en, unsigned int l2ID, unsigned int l3ID)
{
  energy.push_back(en);
  level2ID.push_back(l2ID); // depth
  level3ID.push_back(l3ID); // channel X-Y
}

int event_dictionary::getNumberOfSingles() {return energy.size();}

std::vector<float> event_dictionary::GetEnergy()
{
  return energy;
}

std::vector<unsigned int> event_dictionary::GetLevel2ID()
{
  return level2ID;
}

std::vector<unsigned int> event_dictionary::GetLevel3ID()
{
  return level3ID;
}
