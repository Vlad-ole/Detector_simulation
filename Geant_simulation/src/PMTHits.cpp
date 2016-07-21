#include "PMTHits.h"
#include "Singleton.h"


PMTHits::PMTHits()
{
	g()->PMT_hits = new Hits(4);
	//N_reg_number.resize(4);
}

void PMTHits::IncrValuebyOne(unsigned short N_detector)
{
	if (N_detector > 3)
		cout << "Error! N_detector (PMT) > 3" << endl;
	else 
	{
		g()->PMT_hits->N_reg_v[N_detector]++;
		//N_reg_number[N_detector]++;
	}	

}


PMTHits::~PMTHits()
{
	const double x_pos_PMT = 152 / 2.0;
	const double y_pos_PMT = x_pos_PMT;

	g()->file_num_of_photons_PMT << 0 << "\t" << -x_pos_PMT << "\t" << 0 << "\t" << g()->PMT_hits->N_reg_v[0] << endl;
	g()->file_num_of_photons_PMT << 1 << "\t" << x_pos_PMT << "\t" << 0 << "\t" << g()->PMT_hits->N_reg_v[1] << endl;
	g()->file_num_of_photons_PMT << 2 << "\t" << 0 << "\t" << -y_pos_PMT << "\t" << g()->PMT_hits->N_reg_v[2] << endl;
	g()->file_num_of_photons_PMT << 3 << "\t" << 0 << "\t" << y_pos_PMT << "\t" << g()->PMT_hits->N_reg_v[3] << endl;
	
}
