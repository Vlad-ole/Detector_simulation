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
	void AddTheta(unsigned short N_detector, double value);

private:
	vector<int> N_reg_number;
	//vector<double> theta_angle;
};

#endif

