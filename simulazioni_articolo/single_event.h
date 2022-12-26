#include <vector>

class single_event
{
private:
  int number_of_events;
  int number_of_singles;
  std::vector<int> parentID_v;
  std::vector<double> times_v;
  std::vector<float> energy_v;
  std::vector<int> level2ID_v;
  std::vector<int> level3ID_v;
  std::vector<double> time_shifts_v;

public:
  single_event();

  void add_event(int parentID,
                 double times,
                 float energy,
                 int level2ID,
                 int level3ID);

  int getNumberOfEvents();
};
