#pragma once

#include <vector>
using namespace std;

class Hits
{
public:
	Hits(int);
	~Hits();

	vector<int> N_reg_v;
	vector<double> xpos_v;
	vector<double> ypos_v;
};

