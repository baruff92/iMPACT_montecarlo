#include <vector>


class event_dictionary
{
private:
  std::vector<float> energy;
  std::vector<unsigned int> level2ID; // plane
  std::vector<unsigned int> level3ID; // finger in plane

public:
  void add_event(float en, unsigned int l2ID, unsigned int l3ID);

  int getNumberOfSingles();

  std::vector<float> GetEnergy();
  std::vector<unsigned int> GetLevel2ID();  // TODO try with const
  std::vector<unsigned int> GetLevel3ID();

};
