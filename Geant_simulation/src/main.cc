#include <time.h>
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#include "Randomize.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "RunAction.hh"
#include "MyExceptionHandler.hh"
#include "PrimaryGeneratorAction.hh"

#include "EventAction.hh"
#include "SteppingAction.hh"


#include <conio.h>
#include <iostream>
#include "Singleton.h"
#include "PMTHits.h"

//root cern
#include "TRandom3.h"

//#include <time.h>

using namespace std;
Singleton* Singleton::single = NULL;

int  nEvents = 0;
int  nUsefulEvents = 0;




int main(int argc, char** argv)
{
	long t1 = clock();

	bool isLowStaticticsAndView = 0;

	int N_runs;
	int N_events_per_run;
	if (isLowStaticticsAndView) { N_runs = /*1*/ 10; N_events_per_run = /*10000*/10; }
	else { N_runs = 200000; N_events_per_run = 100; }

		
	//g()->radius_THGEM_hole = 0.25; g()->step_THGEM_hole = 0.9; g()->width_THGEM1 = 0.4; //[mm] //THGEM CERN 28%
	g()->radius_THGEM_hole = 0.5; g()->step_THGEM_hole = 1.1; g()->width_THGEM1 = 1.0;//[mm] //THGEM Electroconnect 75%
	g()->width_THGEM0 = g()->width_THGEM1;
	g()->xyz_position_SingleTHGEMHole = 150;

	//g()->EL_gap_thickness = 13;//double phase
	g()->EL_gap_thickness = 0;//single phase
	g()->z_bottom_THGEM0 = 50.2 + (0.5 - g()->width_THGEM1);
	g()->z_bottom_THGEM1 = /*72.7*/ g()->z_bottom_THGEM0 + g()->width_THGEM0 + 22;

	g()->is_optical_gamma = true;
	g()->is_gamma = false;

	//choose source
	g()->is_Am_coll_14mm = false;
	g()->is_Cd_standard_box = false;
	g()->is_X_ray_coll_35mm_no_alpha = false;
	g()->is_X_ray_coll_14mm_no_alpha = true;//
	g()->is_X_ray_coll_2mm_no_alpha = false;
	g()->is_X_ray_coll_35mm_with_alpha = false;
	g()->is_alpha = false;
	g()->is_point_source = false;

	if ( (g()->is_Am_coll_14mm + g()->is_Cd_standard_box +
		g()->is_X_ray_coll_35mm_no_alpha + g()->is_X_ray_coll_35mm_with_alpha + 
		g()->is_alpha + g()->is_point_source + g()->is_X_ray_coll_2mm_no_alpha + g()->is_X_ray_coll_14mm_no_alpha) > 1 )
	{
		cerr << "Error: choose only one source" << endl;
		std::system("pause");
		exit(1);
	}

	// Choose the Random engine
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

	// Seed the random number generator manually
	// -----------------------------------------
	G4long myseed = time(NULL);
	CLHEP::HepRandom::setTheSeed(myseed);
	cout << "Current time used as seed: " << myseed << G4endl;
	CLHEP::HepRandom::showEngineStatus();


	// Construct the default run manager
	G4RunManager * runManager = new G4RunManager;
	MyExceptionHandler* myExepHand = new MyExceptionHandler();


	//	// set mandatory initialization classes
	DetectorConstruction* detector = new DetectorConstruction;
	runManager->SetUserInitialization(detector);
	runManager->SetUserInitialization(new PhysicsList);

	// set user action classes
	runManager->SetUserAction(new RunAction);

	string temp_string = g()->path_read + "x_ray\\Analytical_model_out.dat";
	if (g()->is_optical_gamma)
	{
		//runManager->SetUserAction(new PrimaryGeneratorAction(g()->string_LAr_T_Heindl_2011.c_str(), 0));
		//runManager->SetUserAction(new PrimaryGeneratorAction(g()->string_GAr_86K_1atm_avalanche_scint_NIR.c_str(), 1));
		runManager->SetUserAction(new PrimaryGeneratorAction());
		g()->avr_N_pe.resize(25);
	}
	else
	{
		runManager->SetUserAction(new PrimaryGeneratorAction());
	}


	EventAction* eventAction = new EventAction;
	runManager->SetUserAction(eventAction);
	runManager->SetUserAction(new SteppingAction(detector, eventAction));
	//
	//Initialize G4 kernel
	runManager->Initialize();

	// visualization manager
#ifdef G4VIS_USE
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
#endif


	// get the pointer to the User Interface manager 
	G4UImanager* UI = G4UImanager::GetUIpointer();





	//for
	//TRandom3 rnd3;
	UI->ApplyCommand("/run/verbose 0");
	UI->ApplyCommand("/event/verbose 0");	
	//UI->ApplyCommand("/tracking/verbose 1"); 
	if (isLowStaticticsAndView) UI->ApplyCommand("/control/execute vis.mac");
	
	for (int i = 0; i < N_runs; i++)
	{
				
		if (i % 100 == 0 || i == (N_runs - 1))
		{
			double val = N_runs > 1 ? (100 * i / (double)(N_runs - 1)) : 100;
			cout << "run = " << i << " (" << val << " %)" << endl;
			cerr << "run = " << i << " (" << val << " %)" << endl;
		}


		g()->CathRefl_index = 0.0;
		g()->SigmaAlpha_index = 0.0741;

		//double val_from = -20;
		//double val_to = 20;
		//double step = 1/*(N_runs > 1) ? (val_to - val_from) / (N_runs - 1) : 0*/;

		//g()->x_source = /*val_from + step*(i)*/ 0;
		//g()->y_source = /*val_from + step*(i)*/ 0;
		//g()->z_source = 54.7;

		//circle
		//const double angle = G4UniformRand() * 3.1415926 * 2;
		//double radius = 11.6 / 2.0;
		//double radius;

		//source-collimator geometry
		//-------------------------------------------
		double h_x = 0; // диаметр источника [mm]
		double lambda_bar;//средняя глубина поглощения в LAr гамма квантов [mm]
		
		// lambda_bar(88 keV) = 33.5 mm
		// lambda_bar(60 keV) = 17.2 mm
		// lambda_bar(25 keV) = 1.7 mm
		if (g()->is_Am_coll_14mm)
		{
			g()->h_c = 6;//14.3 or 6 
			g()->l_x = 14.8;
			h_x = 0;
			lambda_bar = 17.2;
		}
		else if (g()->is_Cd_standard_box)
		{
			g()->h_c = 6; // диаметр коллиматора [mm]
			g()->l_x = 64; // расстояние от источника до коллиматора [mm]
			h_x = 1;
		}
		else if (g()->is_X_ray_coll_35mm_no_alpha)
		{
			g()->h_c = 35;
			g()->l_x = 112;
			h_x = 2;
			lambda_bar = 1.7;
		}
		else if (g()->is_X_ray_coll_14mm_no_alpha)
		{
			g()->h_c = 14;
			g()->l_x = 112;
			h_x = 2;
			lambda_bar = 1.7;
		}
		else if (g()->is_X_ray_coll_2mm_no_alpha)
		{
			g()->h_c = 2;
			g()->l_x = 112;
			h_x = 2;
			//lambda_bar = 1.7;//25keV
			lambda_bar = 6.4;//40keV
		}
		else if (g()->is_point_source)
		{
			g()->x_source = 0;
			g()->y_source = 0;
			g()->z_source = 0;
		}
		else if (g()->is_alpha)
		{
	
		}
		else
		{
			cerr << "Error: this case has not yet been implemented" << endl;
			std::system("pause");
			exit(1);
		}

		

		if (g()->is_optical_gamma && !g()->is_point_source)
		{
			// глубина поглощения в LAr гамма квантов [mm]
			while (true)
			{
				g()->lambda = g()->rnd3.Exp(lambda_bar);
				if (g()->lambda < 50)
					break;
			}
			g()->z_source = g()->lambda;
			
			const double PMMA_width = 3;
			const double LAr_dead_width = 2;
			const double THGEM_Cathode_width = 0.5;
			const double Al_window_width = 23;
			g()->l_L = g()->lambda + PMMA_width + LAr_dead_width + THGEM_Cathode_width + Al_window_width; // расстояние от коллиматора до экрана [mm] 
			//cout << "l_L = " << l_L << endl;

			g()->h_s = (g()->l_L + g()->l_x * g()->h_c / (g()->h_c + h_x)) / (g()->l_x / (g()->h_c + h_x)); //ожидаемый диаметр пятна
			//cout << "h_s = " << g()->h_s << endl;
			g()->radius = g()->h_s / 2.0;

			if (g()->is_alpha)
			{
				g()->radius = 11.5 / 2.0;
				g()->z_source = 2;
			}

			double x_offset = 0;
			double y_offset = 0;
			while (true)
			{
				double x_tmp = (G4UniformRand() - 0.5) * 2 * g()->radius;
				double y_tmp = (G4UniformRand() - 0.5) * 2 * g()->radius;
				if ( x_tmp*x_tmp + y_tmp*y_tmp < (g()->radius)*(g()->radius) )
				{
					g()->x_source = x_tmp + x_offset;
					g()->y_source = y_tmp + y_offset;
					break;
				}
			}
		}

		g()->file_xyz_source << g()->x_source << "\t" << g()->y_source << "\t" << g()->z_source << endl;

		//UI->ApplyCommand("/run/beamOn 1");
		ostringstream osst;
		osst << "/run/beamOn " << N_events_per_run;
		UI->ApplyCommand(osst.str());
		
		//if (argc == 1)
		//{
		//	//if(isLowStaticticsAndView) UI->ApplyCommand("/control/execute run_view.mac");
		//	//else UI->ApplyCommand("/control/execute run_statictics.mac");
		//}
		//else
		//{
		//	// Batch mode
		//	G4String command = "/control/execute ";
		//	G4String fileName = argv[1];
		//	UI->ApplyCommand(command + fileName);
		//}
	}//for

	if(isLowStaticticsAndView) UI->ApplyCommand("vis/viewer/update");
	 ///*out*/
	 //{
	 //	//-----------------------
	 //	//row1
	 //	g()->file_num_of_photons_in_event_SiPM << g()->avr_N_pe[0]/N_runs /*ch32*/ << " " << g()->avr_N_pe[1] / N_runs  /*ch33*/ << " " << g()->avr_N_pe[2] / N_runs  /*ch48*/ << " " << g()->avr_N_pe[3] / N_runs  /*ch49*/ << " " << g()->avr_N_pe[4] / N_runs  /*ch34*/ << endl;
	 //
	 //	//row2
	 //	g()->file_num_of_photons_in_event_SiPM << g()->avr_N_pe[5] / N_runs /*ch35*/ << " " << g()->avr_N_pe[6] / N_runs  /*ch50*/ << " " << g()->avr_N_pe[7] / N_runs  /*ch51*/ << " " << g()->avr_N_pe[8] / N_runs  /*ch36*/ << " " << g()->avr_N_pe[9] / N_runs  /*ch37*/ << endl;
	 //
	 //	//row3
	 //	g()->file_num_of_photons_in_event_SiPM << g()->avr_N_pe[10] / N_runs /*ch52*/ << " " << g()->avr_N_pe[11] / N_runs  /*ch53*/ << " " << g()->avr_N_pe[12] / N_runs  /*ch38*/ << " " << g()->avr_N_pe[13] / N_runs  /*ch39*/ << " " << g()->avr_N_pe[14] / N_runs  /*ch54*/ << endl;
	 //
	 //	//row4
	 //	g()->file_num_of_photons_in_event_SiPM << g()->avr_N_pe[15] / N_runs /*ch55*/ << " " << g()->avr_N_pe[16] / N_runs  /*ch40*/ << " " << g()->avr_N_pe[17] / N_runs  /*ch41*/ << " " << g()->avr_N_pe[18] / N_runs  /*ch56*/ << " " << g()->avr_N_pe[19] / N_runs  /*ch57*/ << endl;
	 //
	 //	//row5
	 //	g()->file_num_of_photons_in_event_SiPM << g()->avr_N_pe[20] / N_runs /*ch42*/ << " " << g()->avr_N_pe[21] / N_runs  /*ch33*/ << " " << g()->avr_N_pe[22] / N_runs  /*ch58*/ << " " << g()->avr_N_pe[23] / N_runs  /*ch59*/ << " " << g()->avr_N_pe[24] / N_runs  /*ch44*/ << endl;
	 //	//-----------------------
	 //	g()->file_num_of_photons_in_event_SiPM << endl;
	 //}



#ifdef G4VIS_USE
	delete visManager;
#endif

	// job termination
	delete runManager;



	//cout << "\a \a \a \a";
	long t2 = clock();

	cout << endl;
	cout << "Lead time is " << (t2 - t1) / 1000.0 << " seconds " << " (or " << (t2 - t1) / 60000.0 << " minutes)" << endl;
	cout << endl;

	system("pause");

	return 0;
}


