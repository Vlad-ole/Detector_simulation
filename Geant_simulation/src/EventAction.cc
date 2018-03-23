#include "EventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4VVisManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"


#include "Scintillation.hh"
#include "Singleton.h"

#include <iostream>
#include <fstream>


EventAction::EventAction() : fTotalEdep(0.)
{
}


EventAction::~EventAction()
{
}

void EventAction::AddtotDetector(G4ThreeVector pos, G4double val)
{
	//file_4 << pos.x()/mm << "\t" << pos.y()/mm << endl;
}


void EventAction::BeginOfEventAction(const G4Event* event)
{
	G4int eventNb = event->GetEventID();

	if (eventNb % 1000 == 0)
	{
		cout << "event = " << eventNb << endl;
	}

	//energy deposited per event
	fTotalEdep = 0.;
	
	//if (eventNb < 10000)
	//{
	//	if (eventNb < 1000)
	//	{
	//		if (eventNb < 10)
	//		{
	//			G4cout << "\n---> Begin of event: " << eventNb << G4endl;
	//		}
	//		else
	//		{
	//			if (eventNb % 10 == 0)
	//				G4cout << "\n---> Begin of event: " << eventNb << G4endl;
	//		}
	//	}
	//	else
	//	{
	//		if (eventNb % 1000 == 0)
	//			G4cout << "\n---> Begin of event: " << eventNb << G4endl;
	//	}
	//}
	//else
	//{
	//	if (eventNb % 10000 == 0)
	//		G4cout << "\n---> Begin of event: " << eventNb << G4endl;
	//}
}


void EventAction::EndOfEventAction(const G4Event* evt)
{
	if (fTotalEdep > 0.) 
	{
		//example from TestEm11
		//Run* run
		//	= static_cast<Run*>(
		//		G4RunManager::GetRunManager()->GetNonConstCurrentRun());
		//run->AddEdep(fTotalEdep);
		//G4AnalysisManager::Instance()->FillH1(2, fTotalEdep);

		g()->file_gamma_event_E_depos << fTotalEdep * 1000 /*E depos in keV*/ << endl;
	}
}

