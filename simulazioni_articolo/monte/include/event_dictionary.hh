#include <vector>


class event_dictionary
{
private:
  std::vector<float> energy;
  std::vector<unsigned int> level2ID; // plane
  std::vector<unsigned int> level3ID; // finger in plane

public:
  void add_event(float en, unsigned int l2ID, unsigned int l3ID);

  int getNumberOfSingles() const;

  std::vector<float> GetEnergy() const;
  std::vector<unsigned int> GetLevel2ID() const;
  std::vector<unsigned int> GetLevel3ID() const;
};
