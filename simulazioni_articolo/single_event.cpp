#include <vector>
#include "single_event.h"

void single_event::add_event(int parentID,
               double times,
               float energy,
               int level2ID,
               int level3ID);
{
  single_event.parentID_v.push_back(parentID);
  single_event.number_of_singles++;
}

int getNumberOfEvents()
{
  return single_event.number_of_singles;
}
