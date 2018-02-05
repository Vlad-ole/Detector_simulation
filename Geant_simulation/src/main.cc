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

	g()->is_optical_gamma = true;

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
		runManager->SetUserAction(new PrimaryGeneratorAction(g()->string_GAr_86K_1atm_avalanche_scint_NIR.c_str(), 1));
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
	TRandom3 rnd3;
	const int N_runs = /*41*/ 1000000 /*1*/;	
	for (int i = 0; i < N_runs; i++)
	{
		if (i % 1 == 0 || i == (N_runs - 1))
		{
			double val = N_runs > 1 ? (100 * i / (double)(N_runs - 1)) : 100;
			cout << "run = " << i << " (" << val << " %)" << endl;
		}


		g()->CathRefl_index = 0.0;
		g()->SigmaAlpha_index = 0.0741;

		double val_from = -20;
		double val_to = 20;
		double step = 1/*(N_runs > 1) ? (val_to - val_from) / (N_runs - 1) : 0*/;

		//g()->x_source = /*val_from + step*(i)*/ 0;
		//g()->y_source = /*val_from + step*(i)*/ 0;
		//g()->z_source = 54.7;

		//circle
		//const double angle = G4UniformRand() * 3.1415926 * 2;
		//double radius = 11.6 / 2.0;
		//double radius;

		//source-collimator geometry
		//-------------------------------------------
		const double h_x = 0; // диаметр источника [mm]
		const double h_c = 2; // диаметр коллиматора [mm]
		const double l_x = 64; // расстояние от источника до коллиматора [mm]

		
		double lambda; // глубина поглощения в LAr гамма квантов [mm]
		while (true)
		{
			lambda = rnd3.Exp(33.5);
			if (lambda < 50)
				break;
		}
		
		const double PMMA_width = 3;
		const double LAr_dead_width = 2;
		const double THGEM_Cathode_width = 0.5;
		const double Al_window_width = 23;
		const double l_L = lambda + PMMA_width + LAr_dead_width + THGEM_Cathode_width + Al_window_width; // расстояние от коллиматора до экрана [mm] 
		//cout << "l_L = " << l_L << endl;

		const double h_s = (l_L + l_x * h_c / (h_c + h_x)) / (l_x / (h_c + h_x)); //ожидаемый диаметр пятна
		//cout << "h_s = " << h_s << endl;
		const double radius = h_s / 2.0;

		//source-collimator geometry end
		//-------------------------------------------
		
		while (true)
		{
			double x_tmp = (G4UniformRand() - 0.5) * 2 * radius;
			double y_tmp = (G4UniformRand() - 0.5) * 2 * radius;
			if (x_tmp*x_tmp + y_tmp*y_tmp < radius*radius)
			{
				g()->x_source = x_tmp;
				g()->y_source = y_tmp;
				g()->file_real_position_of_source << g()->x_source << " " << g()->y_source << " " << lambda << " " << radius << endl;
				break;
			}
		}

		if (argc == 1)
		{
			UI->ApplyCommand("/control/execute slava.mac");
		}
		else
		{
			// Batch mode
			G4String command = "/control/execute ";
			G4String fileName = argv[1];
			UI->ApplyCommand(command + fileName);
		}
	}//for

	



#ifdef G4VIS_USE
	delete visManager;
#endif

	// job termination
	delete runManager;



	cout << "\a \a \a \a";
	long t2 = clock();

	cout << endl;
	cout << "Lead time is " << (t2 - t1) / 1000.0 << " seconds " << " (or " << (t2 - t1) / 60000.0 << " minutes)" << endl;
	cout << endl;

	system("pause");

	return 0;
}


