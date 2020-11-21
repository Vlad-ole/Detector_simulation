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
	SiPM_hits_all_runs = new Hits(11 * 11);
	SiPM_hits_all_runs_LRF = new Hits(11 * 11);
	N_runs_used_for_LRF = 0;
}

RunAction::~RunAction()
{
	delete timer;
	//g()->SiPM_hits->N_reg_v.clear();
	//g()->SiPM_hits->xpos_v.clear();
	//g()->SiPM_hits->ypos_v.clear();
	//cout << "RunAction::~RunAction()" << endl;
	Print(SiPM_hits_all_runs, g()->file_PE_map_SiPM);
	Print(SiPM_hits_all_runs_LRF, g()->file_LRF_SiPM);	
	g()->file_LRF_SiPM << "\n N_runs_used_for_LRF = " << N_runs_used_for_LRF << endl;
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

	bool is_cog = false;
	if (is_cog)
	{
		for (int i = 0; i < g()->SiPM_hits->N_reg_v.size(); i++)
		{
			//central SiPM
			//if (i == 60)

			// condition to choose 5x5 matrix
			if (g()->SiPM_hits->xpos_v[i] > -2.5*step && g()->SiPM_hits->xpos_v[i] < 2.5*step &&
				g()->SiPM_hits->ypos_v[i] > -2.5*step && g()->SiPM_hits->ypos_v[i] < 2.5*step)
			{
				if (i != 40)
				{
					x_mean += g()->SiPM_hits->N_reg_v[i] * g()->SiPM_hits->xpos_v[i];
					y_mean += g()->SiPM_hits->N_reg_v[i] * g()->SiPM_hits->ypos_v[i];
					q_mean += g()->SiPM_hits->N_reg_v[i];
					////cout << i << "\t x_pos = " << g()->SiPM_hits->xpos_v[i] << "\t y_pos = " << g()->SiPM_hits->ypos_v[i] <<"\t N_reg = " << g()->SiPM_hits->N_reg_v[i] << "\t x_mean =  " << x_mean << "\t y_mean = " << y_mean << "\t q_mean = " << q_mean << endl;
					//g()->file_num_of_photons_SiPM << i << "\t" << g()->SiPM_hits->xpos_v[i] << "\t" << g()->SiPM_hits->ypos_v[i] << "\t" << g()->SiPM_hits->N_reg_v[i] << endl;
				}
				else//right-bottom ch in 5x5 matrix is avg of adjacent SiPMs
				{
					double N_pe = (g()->SiPM_hits->N_reg_v[39] + g()->SiPM_hits->N_reg_v[51] + g()->SiPM_hits->N_reg_v[50] / sqrt(2)) / 3.0;
					g()->SiPM_hits->N_reg_v[40] = N_pe;
					x_mean += N_pe * g()->SiPM_hits->xpos_v[i];
					y_mean += N_pe * g()->SiPM_hits->ypos_v[i];
					q_mean += N_pe;
				}
				//g()->file_num_of_photons_in_event_SiPM << g()->SiPM_hits->N_reg_v[i] << " ";			
			}
		}
		if (q_mean != 0)
		{
			x_mean /= q_mean;
			y_mean /= q_mean;

			//cout << "x_mean = " << x_mean << " \t y_mean = " << y_mean << endl;
			g()->file_coord_from_SiPM << x_mean << "\t" << y_mean << endl;
			g()->file_total_num_of_reg_photons_SiPM << q_mean << endl;
			g()->file_real_position_of_source << g()->x_source << " " << g()->y_source << " " << g()->lambda << " " << g()->radius << endl;
		}
	}
	else
	{ 
		//g()->SiPM_hits->N_reg_v[40] = (g()->SiPM_hits->N_reg_v[39] + g()->SiPM_hits->N_reg_v[51] + g()->SiPM_hits->N_reg_v[50] / sqrt(2)) / 3.0;
		g()->SiPM_hits->N_reg_v[40] = (g()->SiPM_hits->N_reg_v[39] + g()->SiPM_hits->N_reg_v[51]) / 2.0;
		int array_index_of_max = -1;
		int max_val = 0;

		for (int i = 0; i < g()->SiPM_hits->N_reg_v.size(); i++)
		{			
			// condition to choose 5x5 matrix
			if (g()->SiPM_hits->xpos_v[i] > -2.5*step && g()->SiPM_hits->xpos_v[i] < 2.5*step &&
				g()->SiPM_hits->ypos_v[i] > -2.5*step && g()->SiPM_hits->ypos_v[i] < 2.5*step)
			{
				//v1
				//q_mean += g()->SiPM_hits->N_reg_v[i];

				//v2 ( exclude bad channels: 43(37), 44(40) )
				if (i != 37 && i != 40)
				{
					q_mean += g()->SiPM_hits->N_reg_v[i];
				}
				
				
				if (g()->SiPM_hits->N_reg_v[i] > max_val)
				{
					max_val = g()->SiPM_hits->N_reg_v[i];
					array_index_of_max = i;
				}

				SiPM_hits_all_runs->N_reg_v[i] += g()->SiPM_hits->N_reg_v[i];

				
			}
		}

		//find "experimental" LRF
		if (array_index_of_max == 60 /*if the maximum number of PE was detected by cental SiPM(ch38)*/)
		{
			N_runs_used_for_LRF++;
			for (int i = 0; i < g()->SiPM_hits->N_reg_v.size(); i++)
			{
				SiPM_hits_all_runs_LRF->N_reg_v[i] += g()->SiPM_hits->N_reg_v[i];
			}
		}


		if (array_index_of_max >= 0)
		{
			g()->file_coord_from_SiPM << g()->SiPM_hits->xpos_v[array_index_of_max] << "\t" << g()->SiPM_hits->ypos_v[array_index_of_max] << endl;
			//g()->file_total_num_of_reg_photons_SiPM << q_mean << endl;
		}

		g()->file_total_num_of_reg_photons_SiPM << q_mean << endl;//print all values, even zero

	}

	Print(g()->SiPM_hits, g()->file_num_of_photons_in_event_SiPM);


	///*cacl summ N_pe*/
	//{
	//	//row1
	//	g()->avr_N_pe[0] += g()->SiPM_hits->N_reg_v[80]; /**/ g()->avr_N_pe[1] += g()->SiPM_hits->N_reg_v[81]; /**/ g()->avr_N_pe[2] += g()->SiPM_hits->N_reg_v[82]; /**/ g()->avr_N_pe[3] += g()->SiPM_hits->N_reg_v[83]; /**/ g()->avr_N_pe[4] += g()->SiPM_hits->N_reg_v[84];

	//	//row2
	//	g()->avr_N_pe[5] += g()->SiPM_hits->N_reg_v[69]; /**/ g()->avr_N_pe[6] += g()->SiPM_hits->N_reg_v[70]; /**/ g()->avr_N_pe[7] += g()->SiPM_hits->N_reg_v[71]; /**/ g()->avr_N_pe[8] += g()->SiPM_hits->N_reg_v[72]; /**/ g()->avr_N_pe[9] += g()->SiPM_hits->N_reg_v[73];

	//	//row3
	//	g()->avr_N_pe[10] += g()->SiPM_hits->N_reg_v[58]; /**/ g()->avr_N_pe[11] += g()->SiPM_hits->N_reg_v[59]; /**/ g()->avr_N_pe[12] += g()->SiPM_hits->N_reg_v[60]; /**/ g()->avr_N_pe[13] += g()->SiPM_hits->N_reg_v[61]; /**/ g()->avr_N_pe[14] += g()->SiPM_hits->N_reg_v[62];

	//	//row4
	//	g()->avr_N_pe[15] += g()->SiPM_hits->N_reg_v[47]; /**/ g()->avr_N_pe[16] += g()->SiPM_hits->N_reg_v[48]; /**/ g()->avr_N_pe[17] += g()->SiPM_hits->N_reg_v[49]; /**/ g()->avr_N_pe[18] += g()->SiPM_hits->N_reg_v[50]; /**/ g()->avr_N_pe[19] += g()->SiPM_hits->N_reg_v[51];

	//	//row5
	//	g()->avr_N_pe[20] += g()->SiPM_hits->N_reg_v[36]; /**/ g()->avr_N_pe[21] += g()->SiPM_hits->N_reg_v[37]; /**/ g()->avr_N_pe[22] += g()->SiPM_hits->N_reg_v[38]; /**/ g()->avr_N_pe[23] += g()->SiPM_hits->N_reg_v[39]; /**/ g()->avr_N_pe[24] += g()->SiPM_hits->N_reg_v[40];
	//}





		
	delete g()->SiPM_hits;
	//cout << " " << *timer << endl;
}


void RunAction::Print(Hits* hits, std::ofstream& file)
{
	/*out*/
	{
		//-----------------------
		//row1
		file << hits->N_reg_v[80] /*ch32*/ << "\t" << hits->N_reg_v[81]  /*ch33*/ << "\t" << hits->N_reg_v[82]  /*ch48*/ << "\t" << hits->N_reg_v[83]  /*ch49*/ << "\t" << hits->N_reg_v[84]  /*ch34*/ << endl;

		//row2
		file << hits->N_reg_v[69] /*ch35*/ << "\t" << hits->N_reg_v[70]  /*ch50*/ << "\t" << hits->N_reg_v[71]  /*ch51*/ << "\t" << hits->N_reg_v[72]  /*ch36*/ << "\t" << hits->N_reg_v[73]  /*ch37*/ << endl;

		//row3
		file << hits->N_reg_v[58] /*ch52*/ << "\t" << hits->N_reg_v[59]  /*ch53*/ << "\t" << hits->N_reg_v[60]  /*ch38*/ << "\t" << hits->N_reg_v[61]  /*ch39*/ << "\t" << hits->N_reg_v[62]  /*ch54*/ << endl;

		//row4
		file << hits->N_reg_v[47] /*ch55*/ << "\t" << hits->N_reg_v[48]  /*ch40*/ << "\t" << hits->N_reg_v[49]  /*ch41*/ << "\t" << hits->N_reg_v[50]  /*ch56*/ << "\t" << hits->N_reg_v[51]  /*ch57*/ << endl;

		//row5
		file << hits->N_reg_v[36] /*ch42*/ << "\t" << hits->N_reg_v[37]  /*ch43*/ << "\t" << hits->N_reg_v[38]  /*ch58*/ << "\t" << hits->N_reg_v[39]  /*ch59*/ << "\t" << hits->N_reg_v[40]  /*ch44*/ << endl;
		//-----------------------
		file << endl;
	}
}
