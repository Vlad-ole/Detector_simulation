#ifndef PMTHits_h
#define PMTHits_h 1

#include <vector>
using namespace std;

class PMTHits
{
public:
	PMTHits();
	~PMTHits();
	void IncrValuebyOne(unsigned short N_detector);

private:
	vector<int> N_reg_number;
};

#endif

