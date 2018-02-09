#include <iostream>
using namespace std;

// Make this appear first!
#include "G4Timer.hh"
//#include "G4INCLRandom.hh"
#include "TRandom3.h"

#include "RunAction.hh"
#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "Singleton.h"


RunAction::RunAction()
{
	timer = new G4Timer;

}

RunAction::~RunAction()
{
	delete timer;
	//g()->SiPM_hits->N_reg_v.clear();
	//g()->SiPM_hits->xpos_v.clear();
	//g()->SiPM_hits->ypos_v.clear();
}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
	//cout << endl;
	//cout << "RunAction::BeginOfRunAction. Run " << aRun->GetRunID() << " start." << endl;
	timer->Start();

	(g()->LightCollection).clear();
	g()->NumberOfReflections = 0;
	g()->NumberOfBornPhotons = 0;
	g()->NumberOfRegPhotons = 0;

	g()->SiPM_hits = new Hits(11 * 11);
}

void RunAction::EndOfRunAction(const G4Run* aRun)
{
	timer->Stop();
	//cout << "RunAction::EndOfRunAction. number of event = " << aRun->GetNumberOfEvent() << endl;
	
	double sum_lc = 0;
	for (int i = 0; i < (g()->LightCollection).size(); i++)
	{
		sum_lc += g()->LightCollection[i];
	}

	
	//cout << sum_lc / (g()->LightCollection).size() << endl;
	//g()->file_run_lc << /*g()->abs_index << "\t" << */ g()->CathRefl_index << "\t" << sum_lc / (g()->LightCollection).size() << endl;
	//g()->file_boundary_process << g()->SigmaAlpha_index << "\t" << ((double) g()->NumberOfReflections) / g()->NumberOfBornPhotons << endl;
	//g()->file_boundary_process << g()->NumberOfRegPhotons << "\t" << ((double) g()->NumberOfReflections) <<  endl;

	const int n_x_SiPM = 11;
	const int y_x_SiPM = 11;
	double x_pos = 0;
	double y_pos = 0;
	double step = 10;
	for (int i = 0; i < g()->SiPM_hits->N_reg_v.size(); i++)
	{		
		x_pos = (i % n_x_SiPM - 5) * step;
		y_pos = (i / y_x_SiPM - 5) * step;

		g()->SiPM_hits->xpos_v[i] = x_pos;
		g()->SiPM_hits->ypos_v[i] = y_pos;
		//cout << i << "\t x_pos = " << x_pos << " \t y_pos = " << y_pos << endl;
	}

	//SiPM coord information
	double x_mean = 0;
	double y_mean = 0;
	double q_mean = 0;


	////add gauss noise
	//const double sigma_part = 3.5;
	//TRandom3 rnd3;
	//for (int i = 0; i < g()->SiPM_hits->N_reg_v.size(); i++)
	//{
	//	if (g()->SiPM_hits->N_reg_v[i] > 0)
	//	{
	//		double Npe_old = g()->SiPM_hits->N_reg_v[i];
	//		double dNpe = rnd3.Gaus(0, (g()->SiPM_hits->N_reg_v[i] * sigma_part));
	//		g()->SiPM_hits->N_reg_v[i] += dNpe;
	//		if (g()->SiPM_hits->N_reg_v[i] < 0)
	//			g()->SiPM_hits->N_reg_v[i] = 0;

	//		//show values
	//		//double Npe_new = g()->SiPM_hits->N_reg_v[i];
	//		//cout << "i = " << i << "; Npe_old = " << Npe_old << "; dNpe = " << dNpe << "; Npe_new = " << Npe_new << endl;
	//	}
	//}


	for (int i = 0; i < g()->SiPM_hits->N_reg_v.size(); i++)
	{
		//central SiPM
		//if (i == 60)
		
		// condition to choose 5x5 matrix
		if (g()->SiPM_hits->xpos_v[i] > -2.5*step && g()->SiPM_hits->xpos_v[i] < 2.5*step && 
			g()->SiPM_hits->ypos_v[i] > -2.5*step && g()->SiPM_hits->ypos_v[i] < 2.5*step)		
		{
			if(i != 40)
			{ 
				x_mean += g()->SiPM_hits->N_reg_v[i] * g()->SiPM_hits->xpos_v[i];
				y_mean += g()->SiPM_hits->N_reg_v[i] * g()->SiPM_hits->ypos_v[i];
				q_mean += g()->SiPM_hits->N_reg_v[i];
				////cout << i << "\t x_pos = " << g()->SiPM_hits->xpos_v[i] << "\t y_pos = " << g()->SiPM_hits->ypos_v[i] <<"\t N_reg = " << g()->SiPM_hits->N_reg_v[i] << "\t x_mean =  " << x_mean << "\t y_mean = " << y_mean << "\t q_mean = " << q_mean << endl;
				//g()->file_num_of_photons_SiPM << i << "\t" << g()->SiPM_hits->xpos_v[i] << "\t" << g()->SiPM_hits->ypos_v[i] << "\t" << g()->SiPM_hits->N_reg_v[i] << endl;
			}
			else//right-bottom ch in 5x5 matrix is avg of adjacent SiPMs
			{
				double N_pe = (g()->SiPM_hits->N_reg_v[39] + g()->SiPM_hits->N_reg_v[51] + g()->SiPM_hits->N_reg_v[50]/sqrt(2))/3.0;
				x_mean += N_pe * g()->SiPM_hits->xpos_v[i];
				y_mean += N_pe * g()->SiPM_hits->ypos_v[i];
				q_mean += N_pe;
			}
		}
	}

	//g()->file_num_of_photons_SiPM << endl << endl;

	if (q_mean != 0 )
	{
		x_mean /= q_mean;
		y_mean /= q_mean;

		//cout << "x_mean = " << x_mean << " \t y_mean = " << y_mean << endl;
		g()->file_coord_from_SiPM << x_mean << "\t" << y_mean << endl;
		g()->file_total_num_of_reg_photons_SiPM << q_mean << endl;
	}


		
	delete g()->SiPM_hits;
	//cout << " " << *timer << endl;
}

