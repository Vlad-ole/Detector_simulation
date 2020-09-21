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

//#define bTHGEM2
//#define bTHGEM1
//#define bTHGEM0

SteppingAction::SteppingAction(DetectorConstruction* myDC, EventAction* myEA)
	:myDetector(myDC), eventAction(myEA)
{
	pmt_hits = new PMTHits();
}

void SteppingAction::UserSteppingAction(const G4Step* theStep)
{
	G4Track* theTrack = theStep->GetTrack();
	G4ParticleDefinition* particleType = theTrack->GetDefinition();

	G4StepPoint* thePrePoint = theStep->GetPreStepPoint();
	G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();

	G4StepPoint* thePostPoint = theStep->GetPostStepPoint();
	G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();
	
	//const G4String thePostPVName = thePostPV->GetName();

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

				const double width = 500 * um;

#ifdef bTHGEM2
				if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_tracker_THGEM2")
				{
					const G4ThreeVector& pos_i = theStep->GetPostStepPoint()->GetPosition();					
					const double z_bottom = 77.2;
					const double z_top = z_bottom + width;
					if (pos_i.z() < (z_top + 0.1)*mm &&  pos_i.z() > (z_top - 0.1)*mm)
					{
						PassThroughGEM(theStep, z_top * mm, width);
					}
					else if (pos_i.z() < (z_bottom + 0.1)*mm &&  pos_i.z() > (z_bottom - 0.1)*mm)
					{
						PassThroughGEM(theStep, z_bottom * mm, width);
					}
				}
#endif // bTHGEM2

				
#ifdef bTHGEM1
				if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_tracker_THGEM1")
				{
					const G4ThreeVector& pos_i = theStep->GetPostStepPoint()->GetPosition();
					const double z_bottom = 72.7;
					const double z_top = z_bottom + width;
					if ( pos_i.z() < (z_top + 0.1)*mm &&  pos_i.z() > (z_top - 0.1)*mm )
					{
						PassThroughGEM(theStep, z_top * mm, width);
					}
					else if (pos_i.z() < (z_bottom + 0.1)*mm &&  pos_i.z() > (z_bottom - 0.1)*mm)
					{
						PassThroughGEM(theStep, z_bottom * mm, width);
					}
				}
#endif // bTHGEM1


#ifdef bTHGEM0

				if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_tracker_THGEM0")
				{
						const G4ThreeVector& pos_i = theStep->GetPostStepPoint()->GetPosition();
						const double z_bottom = 50.2;
						const double z_top = z_bottom + width;
						if ( pos_i.z() < (z_top + 0.1)*mm &&  pos_i.z() > (z_top - 0.1)*mm )
						{
							PassThroughGEM(theStep, z_top * mm, width);
						}
						else if (pos_i.z() < (z_bottom + 0.1)*mm &&  pos_i.z() > (z_bottom - 0.1)*mm)
						{
							PassThroughGEM(theStep, z_bottom * mm, width);
						}					
				}

#endif // bTHGEM0

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

	//For gamma or e-
	if (particleType == G4Gamma::GammaDefinition() || particleType == G4Electron::ElectronDefinition())
	{
		if (thePostPV != NULL) //just to avoid "OutOfWorld" volume
		{
			G4ThreeVector position = thePostPoint->GetPosition();

			//////choose active LAr
			//if (thePostPV->GetName() == "phys_LAr_inner" || thePostPV->GetName() == "phys_tracker_THGEM0" ||
			//	thePostPV->GetName() == "phys_FieldTHGEM_1" || thePostPV->GetName() == "phys_FieldTHGEM_2")
			if (thePostPV->GetName() == "phys_Cd109Detector" )
			{
				//cout << "vlad_info ******************; " << particleType->GetParticleType() << endl;
				G4double edep = theStep->GetTotalEnergyDeposit();
				if (edep > 0)
				{
					//cout << "vlad_info " << particleType->GetParticleType() << " " << 
						//position.x() << " " << position.y() << " " << position.z() << " " <<
						//edep <<
						//endl;

					//total energy deposit in absorber
					eventAction->AddEdep(edep);
					//g()->file_gamma_step_E_depos << edep * 1000 /*E in keV*/ << endl;
				}				
			}

		}
		
	}
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
			const double sign = MomentumDirection_i.z() > 0 ? 1 : -1 ;
			const G4ThreeVector& Position_f = G4ThreeVector(Position_i.x() + dx, Position_i.y() + dy, (z_pos + z_size*sign) );

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


