#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4SteppingManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ProcessManager.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4OpBoundaryProcess.hh"

#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "CathodeSD.hh"
#include "PMTSD.h"

#include "Singleton.h"
#include "PMTHits.h"
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction(DetectorConstruction* myDC, EventAction* myEA)
	:myDetector(myDC), eventAction(myEA)
{
	pmt_hits = new PMTHits();
}

void SteppingAction::UserSteppingAction(const G4Step* theStep)
{
	G4Track* theTrack = theStep->GetTrack();

	G4StepPoint* thePrePoint = theStep->GetPreStepPoint();
	G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();

	G4StepPoint* thePostPoint = theStep->GetPostStepPoint();
	G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();

	//---------------------


	//---------------------


	//find the boundary process only once
	static G4OpBoundaryProcess* boundary = NULL;
	if (!boundary)
	{
		G4ProcessManager* pm = theStep->GetTrack()->GetDefinition()->GetProcessManager();
		G4int nprocesses = pm->GetProcessListLength();
		G4ProcessVector* pv = pm->GetProcessList();

		for (G4int i = 0; i < nprocesses; i++)
		{
			if ((*pv)[i]->GetProcessName() == "OpBoundary")
			{
				boundary = (G4OpBoundaryProcess*)(*pv)[i];
				break;
			}
		}
	}


	//For Optical photon only
	G4ParticleDefinition* particleType = theTrack->GetDefinition();
	if (particleType == G4OpticalPhoton::OpticalPhotonDefinition())
	{

		//Check to see if the partcile was actually at a boundary
		//Otherwise the boundary status may not be valid
		//Prior to Geant4.6.0-p1 this would not have been enough to check
		if (thePostPoint->GetStepStatus() == fGeomBoundary)
		{
			if (boundary)
			{
				G4OpBoundaryProcessStatus boundaryStatus = boundary->GetStatus();

				if (boundaryStatus == FresnelReflection)
					g()->NumberOfReflections++;



				if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_tracker_THGEM2")
				{
					PassThroughGEM(theStep, 70.7 * mm, 500 * um);
				}

				if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_tracker_THGEM1")
				{
					PassThroughGEM(theStep, 66.2 * mm, 500 * um);
				}

				if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_tracker_THGEM0")
				{
					PassThroughGEM(theStep, 50.2 * mm, 500 * um);
				}

				switch (boundaryStatus)
				{
				case Detection://Note, this assumes that the volume causing detection
								//is the photocathode because it is the only one with
								//non-zero efficiency

								//Triger sensitive detector manually since photon is
								//absorbed but status was Detection
				{

					if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_PMT0")
						pmt_hits->IncrValuebyOne(0);
					else if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_PMT1")
						pmt_hits->IncrValuebyOne(1);
					else if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_PMT2")
						pmt_hits->IncrValuebyOne(2);
					else if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_PMT3")
						pmt_hits->IncrValuebyOne(3);
					else
					{
						G4SDManager* SDman = G4SDManager::GetSDMpointer();
						G4String sdName = "/detector/sensitiveDetector";
						CathodeSD* sipmSD = (CathodeSD*)SDman->FindSensitiveDetector(sdName);
						if (sipmSD)sipmSD->ProcessHits_Optical(theStep, NULL);
					}

					//PMTSD* pmtSD = (PMTSD*)SDman->FindSensitiveDetector("/detector/sensitiveDetector_PMT0");
					//if (pmtSD)pmtSD->ProcessHits_Optical(theStep, NULL);

				}
				break;

				case Absorption:
				case FresnelReflection:
				case TotalInternalReflection:
				case LambertianReflection:
				case LobeReflection:
				case SpikeReflection:
				case BackScattering:
					break;

				default:
					break;
				}
			}
		}//if Boundary
	}//if opt photons
}




void SteppingAction::PassThroughGEM(const G4Step* theStep, G4double z_pos, G4double z_size)
{
	//change MomentumDirection and position for optical photon incidents on THGEM

	const G4ThreeVector& pos_i = theStep->GetPostStepPoint()->GetPosition();
	if (pos_i.z() > (z_pos - 0.1) && pos_i.z() < (z_pos + 0.1))
	{
		//get
		const G4ThreeVector& MomentumDirection_i = theStep->GetTrack()->GetMomentumDirection();
		const G4ThreeVector& Position_i = theStep->GetTrack()->GetPosition();

		const double step = 0.9 * mm;
		const double step_y = step * sqrt(3) / 2;
		const double radius = 0.25 * mm;

		double y_center;
		double x_center;

		// the nearest y_center
		if (fabs(Position_i.y()) < (step_y / 2))
		{
			y_center = 0;
		}
		else
		{
			y_center = ((int)((fabs(Position_i.y()) - (step_y / 2)) / (step_y)) + 1) * (step_y)* Position_i.y() / fabs(Position_i.y());
		}

		//the nearest x_center
		if (fabs(Position_i.y()) < (step_y / 2))
		{
			int n_x = (int)(Position_i.x() / step);
			int sign = Position_i.x() > 0 ? 1 : -1;
			x_center = (n_x + sign * 0.5) * step;
		}
		else
		{
			if (((int)((fabs(Position_i.y()) - (step_y / 2)) / (step_y)) + 1) % 2 == 1 /*if y row is odd*/)
			{
				if (fabs(Position_i.x()) < step / 2.0)
				{
					x_center = 0;
				}
				else
				{
					int n_x = (fabs(Position_i.x()) - step / 2) / step;
					int sign = Position_i.x() > 0 ? 1 : -1;
					x_center = (n_x + 1) * step * sign;
				}
			}
			else
			{
				int n_x = (int)(Position_i.x() / step);
				int sign = Position_i.x() > 0 ? 1 : -1;
				x_center = (n_x + sign * 0.5) * step;
			}
		}

		//kill or allow photon to pass
		double distance = sqrt(pow(Position_i.x() - x_center, 2.0) + pow(Position_i.y() - y_center, 2.0));
		if (distance < radius)
		{
			//set
			const G4ThreeVector& MomentumDirection_f = G4ThreeVector(0, 1, 0);
			const double dx = MomentumDirection_i.x() * 0.5*mm / sqrt(MomentumDirection_i.x() * MomentumDirection_i.x() + MomentumDirection_i.z() * MomentumDirection_i.z());
			const double dy = MomentumDirection_i.y() * 0.5*mm / sqrt(MomentumDirection_i.y() * MomentumDirection_i.y() + MomentumDirection_i.z() * MomentumDirection_i.z());
			const G4ThreeVector& Position_f = G4ThreeVector(Position_i.x() + dx, Position_i.y() + dy, (z_pos + z_size) );

			distance = sqrt(pow(Position_f.x() - x_center, 2.0) + pow(Position_f.y() - y_center, 2.0));
			if (distance < radius)
			{
				theStep->GetTrack()->SetPosition(Position_f);
				//theStep->GetTrack()->SetMomentumDirection(MomentumDirection_i); // I see strange result and don't know why
				//cout << "InHole !" << "x_center = " << x_center << " \t y_center = " << y_center << endl;
			}
			else
			{
				theStep->GetTrack()->SetTrackStatus(fStopAndKill);
				//system("pause");
			}

		}
		else
		{
			//cout << "OutHole !" << "x_center = " << x_center << " \t y_center = " << y_center << endl;
			theStep->GetTrack()->SetTrackStatus(fStopAndKill);
		}



	}

}


