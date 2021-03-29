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
#define bTHGEM1
//#define bTHGEM0
//#define Cu_REFLECTION
//#define bPASSTHROUGHTHGEMsREFLECTION

SteppingAction::SteppingAction(DetectorConstruction* myDC, EventAction* myEA)
	:myDetector(myDC), eventAction(myEA)
{
	pmt_hits = new PMTHits();
	delta_z = 0.01;
}

void SteppingAction::UserSteppingAction(const G4Step* theStep)
{
	G4Track* theTrack = theStep->GetTrack();
	G4ParticleDefinition* particleType = theTrack->GetDefinition();

	G4StepPoint* thePrePoint = theStep->GetPreStepPoint();
	G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();

	G4StepPoint* thePostPoint = theStep->GetPostStepPoint();
	G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();


	//const G4ThreeVector& pos_f_h = theStep->GetPostStepPoint()->GetPosition();//final position inside SingleTHGEMHole
	//cout << "pos_f_h = " << pos_f_h.x() << "\t" << pos_f_h.y() << "\t" << pos_f_h.z() << endl;
	
	//const G4String thePostPVName = thePostPV->GetName();

	//---------------------
	if (thePostPV != NULL) 
	{	
		const G4ThreeVector& pos = theStep->GetPostStepPoint()->GetPosition();
		string VolName = theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
		const G4ThreeVector& MomentumDirection = theStep->GetTrack()->GetMomentumDirection();
		//cout << "VolName, xyz, m_xyz =" << VolName << "\t" << pos.x() << "\t" << pos.y() << "\t" << pos.z() <<
		//	"\t; Moment = " << MomentumDirection.x() << "\t" << MomentumDirection.y() << "\t" << MomentumDirection.z() << endl;
	}
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

				//const double width = 500 * um;

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
				//if photon go to THGEM1
				if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_tracker_THGEM1")
				{
					const G4ThreeVector& pos_i = theStep->GetPostStepPoint()->GetPosition();
					const double z_bottom = g()->z_bottom_THGEM1 /*72.7*/;
					const double z_top = z_bottom + g()->width_THGEM1;
					if ( pos_i.z() < (z_top + delta_z)*mm &&  pos_i.z() > (z_top - delta_z)*mm )
					{
						PassThroughGEM(theStep, z_top * mm, g()->width_THGEM1);
					}
					else if (pos_i.z() < (z_bottom + delta_z)*mm &&  pos_i.z() > (z_bottom - delta_z)*mm)
					{
						PassThroughGEM(theStep, z_bottom * mm, g()->width_THGEM1);
					}
				}

				


#endif // bTHGEM1


#ifdef bTHGEM0

				if (theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_tracker_THGEM0")
				{
						const G4ThreeVector& pos_i = theStep->GetPostStepPoint()->GetPosition();
						const double z_bottom = g()->z_bottom_THGEM0 /*50.2*/;
						const double z_top = z_bottom + g()->width_THGEM0;
						if ( pos_i.z() < (z_top + delta_z)*mm &&  pos_i.z() > (z_top - delta_z)*mm )
						{
							PassThroughGEM(theStep, z_top * mm, g()->width_THGEM0);
						}
						else if (pos_i.z() < (z_bottom + delta_z)*mm &&  pos_i.z() > (z_bottom - delta_z)*mm)
						{
							PassThroughGEM(theStep, z_bottom * mm, g()->width_THGEM0);
						}					
				}

				

#endif // bTHGEM0

				#ifdef bPASSTHROUGHTHGEMsREFLECTION
				ReturnFromSingleTHGEMHole(theStep);
				#endif //bPASSTHROUGHTHGEMsREFLECTION
				

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

	//cout << endl;
}

void SteppingAction::ReturnFromSingleTHGEMHole(const G4Step* theStep)
{	
	G4double z_size;
	
	if (Volume_init_THGEM == "phys_tracker_THGEM1")
	{
		z_size = g()->width_THGEM1;
	}
	
	if (Volume_init_THGEM == "phys_tracker_THGEM0")
	{
		z_size = g()->width_THGEM0;
	}
	
	
	
	//final position inside SingleTHGEMHole
	const G4ThreeVector& pos_f_h = theStep->GetPostStepPoint()->GetPosition();
	const G4ThreeVector& mom_f_h = theStep->GetTrack()->GetMomentumDirection();
	bool is_bottom_to_top = (pos_f_h.z() == g()->xyz_position_SingleTHGEMHole + z_size / 2.0) && mom_f_h.z() > 0;
	bool is_top_to_bottom = (pos_f_h.z() == g()->xyz_position_SingleTHGEMHole - z_size / 2.0) && mom_f_h.z() < 0;
	//if photon go out from SingleTHGEMHole
	if (is_photon_inside_single_THGEM_hole && (is_bottom_to_top || is_top_to_bottom))
	{
		is_photon_inside_single_THGEM_hole = false;

		//find the x,y shift in the SingleTHGEMHole
		double dx = pos_f_h.x() - Position_init_SingleTHGEMHole.x();
		double dy = pos_f_h.y() - Position_init_SingleTHGEMHole.y();
		
		
		const G4ThreeVector& MomentumDirection_f_THGEM = theStep->GetTrack()->GetMomentumDirection();
		double zm = MomentumDirection_f_THGEM.z();
		double zm_sign = zm > 0 ? 1 : -1;

		bool is_changed_direction = (MomentumDirection_init_THGEM.z() > 0) == !(MomentumDirection_f_THGEM.z()>0); /* a==!b means XOR*/
		double dz = is_changed_direction ? 0 : z_size*zm_sign;

		const G4ThreeVector& pos_f_THGEM =
			G4ThreeVector(Position_init_THGEM.x() + dx, Position_init_THGEM.y() + dy,
				Position_init_THGEM.z() + dz - zm_sign*delta_z);

		/*cout << "zm = " << zm << endl;
		cout << "zm_sign = " << zm_sign << endl;
		cout << "is_changed_direction = " << is_changed_direction << endl;
		cout << "dz = " << dz << endl;*/

		theStep->GetTrack()->SetPosition(pos_f_THGEM);
	}
}

//v1
//void SteppingAction::PassThroughGEM(const G4Step* theStep, G4double z_pos, G4double z_size)
//{
//	//change MomentumDirection and position for optical photon incidents on THGEM
//
//	const G4ThreeVector& pos_i = theStep->GetPostStepPoint()->GetPosition();
//	if (pos_i.z() > (z_pos - delta_z) && pos_i.z() < (z_pos + delta_z))
//	{
//		//get
//		const G4ThreeVector& MomentumDirection_i = theStep->GetTrack()->GetMomentumDirection();
//		const G4ThreeVector& Position_i = theStep->GetTrack()->GetPosition();
//
//		//THGEM CERN 28%
//		const double step = 0.9 * mm;
//		//const double radius = 0.25 * mm;
//
//		////THGEM ELECROCONNECT 75%
//		//const double step = 1.1 * mm;
//		//const double radius = 0.5 * mm;
//
//		const double step_y = step * sqrt(3) / 2;
//		double y_center;
//		double x_center;
//
//		// the nearest y_center
//		if (fabs(Position_i.y()) < (step_y / 2))
//		{
//			y_center = 0;
//		}
//		else
//		{
//			y_center = ((int)((fabs(Position_i.y()) - (step_y / 2)) / (step_y)) + 1) * (step_y)* Position_i.y() / fabs(Position_i.y());
//		}
//
//		//the nearest x_center
//		if (fabs(Position_i.y()) < (step_y / 2))
//		{
//			int n_x = (int)(Position_i.x() / step);
//			int sign = Position_i.x() > 0 ? 1 : -1;
//			x_center = (n_x + sign * 0.5) * step;
//		}
//		else
//		{
//			if (((int)((fabs(Position_i.y()) - (step_y / 2)) / (step_y)) + 1) % 2 == 1 /*if y row is odd*/)
//			{
//				if (fabs(Position_i.x()) < step / 2.0)
//				{
//					x_center = 0;
//				}
//				else
//				{
//					int n_x = (fabs(Position_i.x()) - step / 2) / step;
//					int sign = Position_i.x() > 0 ? 1 : -1;
//					x_center = (n_x + 1) * step * sign;
//				}
//			}
//			else
//			{
//				int n_x = (int)(Position_i.x() / step);
//				int sign = Position_i.x() > 0 ? 1 : -1;
//				x_center = (n_x + sign * 0.5) * step;
//			}
//		}
//
//		//kill or allow photon to pass
//		double distance = sqrt(pow(Position_i.x() - x_center, 2.0) + pow(Position_i.y() - y_center, 2.0));
//		if (distance < g()->radius_THGEM_hole)
//		{
//			//set
//			const G4ThreeVector& MomentumDirection_f = G4ThreeVector(0, 1, 0);
//			const double dx = MomentumDirection_i.x() * 0.5*mm / sqrt(MomentumDirection_i.x() * MomentumDirection_i.x() + MomentumDirection_i.z() * MomentumDirection_i.z());
//			const double dy = MomentumDirection_i.y() * 0.5*mm / sqrt(MomentumDirection_i.y() * MomentumDirection_i.y() + MomentumDirection_i.z() * MomentumDirection_i.z());
//			const double sign = MomentumDirection_i.z() > 0 ? 1 : -1 ;
//			const G4ThreeVector& Position_f = G4ThreeVector(Position_i.x() + dx, Position_i.y() + dy, (z_pos + z_size*sign) );
//
//			distance = sqrt(pow(Position_f.x() - x_center, 2.0) + pow(Position_f.y() - y_center, 2.0));
//			if (distance < g()->radius_THGEM_hole)
//			{
//				theStep->GetTrack()->SetPosition(Position_f);
//				//theStep->GetTrack()->SetMomentumDirection(MomentumDirection_i); // I see strange result and don't know why
//				//cout << "InHole !" << "x_center = " << x_center << " \t y_center = " << y_center << endl;
//			}
//			else
//			{
//				theStep->GetTrack()->SetTrackStatus(fStopAndKill);
//				//system("pause");
//			}
//
//		}
//		else
//		{
//			//cout << "OutHole !" << "x_center = " << x_center << " \t y_center = " << y_center << endl;
//			theStep->GetTrack()->SetTrackStatus(fStopAndKill);
//		}
//
//
//
//	}
//
//}

//#ifndef bPASSTHROUGHTHGEMsREFLECTION
////v2
//void SteppingAction::PassThroughGEM(const G4Step* theStep, G4double z_pos, G4double z_size)
//{
//	//change MomentumDirection and position for optical photon incidents on THGEM
//
//	const G4ThreeVector& pos_i = theStep->GetPostStepPoint()->GetPosition();
//	if (pos_i.z() > (z_pos - delta_z) && pos_i.z() < (z_pos + delta_z))
//	{
//		//get
//		const G4ThreeVector& MomentumDirection_i = theStep->GetTrack()->GetMomentumDirection();
//		const G4ThreeVector& Position_i = theStep->GetTrack()->GetPosition();
//		
//		//THGEM CERN 28%
//		const double step = 0.9 * mm;
//		//const double radius = 0.25 * mm;
//
//		////THGEM ELECROCONNECT 75%
//		//const double step = 1.1 * mm;
//		//const double radius = 0.5 * mm;
//
//		const double step_y = step * sqrt(3) / 2;
//		double y_center;
//		double x_center;
//
//		double x = Position_i.x();
//		double y = Position_i.y();
//		double x_abs = fabs(Position_i.x());
//		double y_abs = fabs(Position_i.y());
//
//		// the nearest y_center
//		if ( y_abs < step_y/2 )
//		{
//			y_center = 0;
//		}
//		else
//		{
//			y_center = ( (int)((y_abs - step_y/2) / step_y) + 1 ) * step_y * y / y_abs;
//		}
//
//		//the nearest x_center
//		if (y_abs < step_y/2)
//		{
//			int n_x = (int)(x / step);
//			int sign = x > 0 ? 1 : -1;
//			x_center = (n_x + sign * 0.5) * step;
//		}
//		else
//		{
//			if ( ((int)((y_abs - (step_y / 2)) / (step_y)) + 1) % 2 == 1 /*if y row is odd*/)
//			{
//				if (x_abs < step / 2.0)
//				{
//					x_center = 0;
//				}
//				else
//				{
//					int n_x = (x_abs - step / 2) / step;
//					int sign = x > 0 ? 1 : -1;
//					x_center = (n_x + 1) * step * sign;
//				}
//			}
//			else
//			{
//				int n_x = (int)(x / step);
//				int sign = x > 0 ? 1 : -1;
//				x_center = (n_x + sign * 0.5) * step;
//			}
//		}
//
//		//kill or allow photon to pass
//		double distance = sqrt(pow(x - x_center, 2.0) + pow(y - y_center, 2.0));
//		if (distance < g()->radius_THGEM_hole)
//		{
//			//set
//			//const G4ThreeVector& MomentumDirection_f = G4ThreeVector(0, 1, 0);
//			double xm = MomentumDirection_i.x();
//			double ym = MomentumDirection_i.y();
//			double zm = MomentumDirection_i.z();
//			
//			/*const double dx = xm * 0.5*mm / sqrt(xm * xm + zm * zm);//probably, incorrect
//			const double dy = ym * 0.5*mm / sqrt(ym * ym + zm * zm);*/
//			
//			const double dx = z_size * xm/zm;//it should be correct
//			const double dy = z_size * ym/zm;
//
//			const double sign = zm > 0 ? 1 : -1;
//			const G4ThreeVector& Position_f = G4ThreeVector(x + dx, y + dy, (z_pos + z_size*sign));
//
//			distance = sqrt(pow(Position_f.x() - x_center, 2.0) + pow(Position_f.y() - y_center, 2.0));
//			if (distance < g()->radius_THGEM_hole)
//			{
//				theStep->GetTrack()->SetPosition(Position_f);
//				//theStep->GetTrack()->SetMomentumDirection(MomentumDirection_i); // I see strange result and don't know why
//				//cout << "InHole !" << "x_center = " << x_center << " \t y_center = " << y_center << endl;
//			}
//			else
//			{
//				theStep->GetTrack()->SetTrackStatus(fStopAndKill);
//				//system("pause");
//			}
//
//		}
//		else
//		{
//			//cout << "OutHole !" << "x_center = " << x_center << " \t y_center = " << y_center << endl;
//			theStep->GetTrack()->SetTrackStatus(fStopAndKill);
//		}
//
//
//
//	}
//
//}
//#endif //!bPASSTHROUGHTHGEMsREFLECTION

#ifndef bPASSTHROUGHTHGEMsREFLECTION
//v2.2
void SteppingAction::PassThroughGEM(const G4Step* theStep, G4double z_pos, G4double z_size)
{
	//change MomentumDirection and position for optical photon incidents on THGEM

	const G4ThreeVector& pos_i = theStep->GetPostStepPoint()->GetPosition();
	if (pos_i.z() > (z_pos - delta_z) && pos_i.z() < (z_pos + delta_z))
	{
		//get
		const G4ThreeVector& MomentumDirection_i = theStep->GetTrack()->GetMomentumDirection();
		const G4ThreeVector& Position_i = theStep->GetTrack()->GetPosition();

		//THGEM CERN 28%
		const double step = 0.9 * mm;
		//const double radius = 0.25 * mm;

		////THGEM ELECROCONNECT 75%
		//const double step = 1.1 * mm;
		//const double radius = 0.5 * mm;

		const double step_y = step * sqrt(3) / 2;
		double y_center;
		double x_center;

		double x = Position_i.x();
		double y = Position_i.y();
		double x_abs = fabs(Position_i.x());
		double y_abs = fabs(Position_i.y());

		// the nearest y_center
		if (y_abs < step_y / 2)
		{
			y_center = 0;
		}
		else
		{
			y_center = ((int)((y_abs - step_y / 2) / step_y) + 1) * step_y * y / y_abs;
		}

		//the nearest x_center
		if (y_abs < step_y / 2)
		{
			int n_x = (int)(x / step);
			int sign = x > 0 ? 1 : -1;
			x_center = (n_x + sign * 0.5) * step;
		}
		else
		{
			if (((int)((y_abs - (step_y / 2)) / (step_y)) + 1) % 2 == 1 /*if y row is odd*/)
			{
				if (x_abs < step / 2.0)
				{
					x_center = 0;
				}
				else
				{
					int n_x = (x_abs - step / 2) / step;
					int sign = x > 0 ? 1 : -1;
					x_center = (n_x + 1) * step * sign;
				}
			}
			else
			{
				int n_x = (int)(x / step);
				int sign = x > 0 ? 1 : -1;
				x_center = (n_x + sign * 0.5) * step;
			}
		}

		//kill or allow photon to pass
		double distance = sqrt(pow(x - x_center, 2.0) + pow(y - y_center, 2.0));
		if (distance < g()->radius_THGEM_hole)
		{
			//set
			//const G4ThreeVector& MomentumDirection_f = G4ThreeVector(0, 1, 0);
			double xm = MomentumDirection_i.x();
			double ym = MomentumDirection_i.y();
			double zm = MomentumDirection_i.z();

			/*const double dx = xm * 0.5*mm / sqrt(xm * xm + zm * zm);//probably, incorrect
			const double dy = ym * 0.5*mm / sqrt(ym * ym + zm * zm);*/

			const double sign = zm > 0 ? 1 : -1;
			const double dx = z_size * xm / (zm*sign);//it should be correct
			const double dy = z_size * ym / (zm*sign);
			
			const G4ThreeVector& Position_f = G4ThreeVector(x + dx, y + dy, (z_pos + z_size*sign));

			distance = sqrt(pow(Position_f.x() - x_center, 2.0) + pow(Position_f.y() - y_center, 2.0));

			//cout << "center: " << x_center << "\t" << y_center << "\t" << endl;
			//cout << "dxy: " << dx << "\t" << dy << endl;
			//cout << "Position_f: " << Position_f.x() << "\t" << Position_f.y() << endl;
			//cout << "distance: " << distance << endl;
			if (distance < g()->radius_THGEM_hole)
			{
				theStep->GetTrack()->SetPosition(Position_f);
				//theStep->GetTrack()->SetMomentumDirection(MomentumDirection_i); // I see strange result and don't know why
				//cout << "InHole !" << "x_center = " << x_center << " \t y_center = " << y_center << endl;
				//cout << "InHole !" << endl;
				//cout << "InHole !" << "Position_f.x() = " << Position_f.x() << " \t Position_f.y()  = " << Position_f.y() <<
					//" \t Position_f.z()  = " << Position_f.z() << endl;
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
			//cout << "distance: " << distance << endl;
			#ifndef Cu_REFLECTION
			theStep->GetTrack()->SetTrackStatus(fStopAndKill);
			#endif //Cu_REFLECTION
		}



	}

}
#endif //!bPASSTHROUGHTHGEMsREFLECTION


#ifdef bPASSTHROUGHTHGEMsREFLECTION
//v3 
void SteppingAction::PassThroughGEM(const G4Step* theStep, G4double z_pos, G4double z_size)
{
	//change MomentumDirection and position for optical photon incidents on THGEM

	const G4ThreeVector& pos_i = theStep->GetPostStepPoint()->GetPosition();
	if (pos_i.z() > (z_pos - delta_z) && pos_i.z() < (z_pos + delta_z))
	{
		//get
		const G4ThreeVector& MomentumDirection_i = theStep->GetTrack()->GetMomentumDirection();
		const G4ThreeVector& Position_i = theStep->GetTrack()->GetPosition();
		Position_init_THGEM = Position_i;

		//THGEM CERN 28%
		const double step = 0.9 * mm;
		//const double radius = 0.25 * mm;

		////THGEM ELECROCONNECT 75%
		//const double step = 1.1 * mm;
		//const double radius = 0.5 * mm;

		const double step_y = step * sqrt(3) / 2;
		double y_center;
		double x_center;

		double x = Position_i.x();
		double y = Position_i.y();
		double x_abs = fabs(Position_i.x());
		double y_abs = fabs(Position_i.y());

		// the nearest y_center
		if (y_abs < step_y / 2)
		{
			y_center = 0;
		}
		else
		{
			y_center = ((int)((y_abs - step_y / 2) / step_y) + 1) * step_y * y / y_abs;
		}

		//the nearest x_center
		if (y_abs < step_y / 2)
		{
			int n_x = (int)(x / step);
			int sign = x > 0 ? 1 : -1;
			x_center = (n_x + sign * 0.5) * step;
		}
		else
		{
			if (((int)((y_abs - (step_y / 2)) / (step_y)) + 1) % 2 == 1 /*if y row is odd*/)
			{
				if (x_abs < step / 2.0)
				{
					x_center = 0;
				}
				else
				{
					int n_x = (x_abs - step / 2) / step;
					int sign = x > 0 ? 1 : -1;
					x_center = (n_x + 1) * step * sign;
				}
			}
			else
			{
				int n_x = (int)(x / step);
				int sign = x > 0 ? 1 : -1;
				x_center = (n_x + sign * 0.5) * step;
			}
		}

		//kill or allow photon to pass
		double distance = sqrt(pow(x - x_center, 2.0) + pow(y - y_center, 2.0));
		//cout << "x - x_center = " << x - x_center << " \t y - y_center = " << y - y_center << endl;
		if (distance < g()->radius_THGEM_hole)
		{
			//set
			//const G4ThreeVector& MomentumDirection_f = G4ThreeVector(0, 1, 0);
			double xm = MomentumDirection_i.x();
			double ym = MomentumDirection_i.y();
			double zm = MomentumDirection_i.z();

			//set new initial position inside SingleTHGEMHole
			double zm_sign = zm > 0 ? 1 : -1;
			const G4ThreeVector& Position_new_i = G4ThreeVector(g()->xyz_position_SingleTHGEMHole + x - x_center, 
				g()->xyz_position_SingleTHGEMHole + y - y_center, g()->xyz_position_SingleTHGEMHole - zm_sign*z_size/2.0);
			Position_init_SingleTHGEMHole = Position_new_i;
			if (distance < g()->radius_THGEM_hole)
			{
				
				MomentumDirection_init_THGEM = MomentumDirection_i;
				Volume_init_THGEM = theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();

				//inside SingleTHGEMHole, start propagation
				is_photon_inside_single_THGEM_hole = true;
				theStep->GetTrack()->SetPosition(Position_new_i);				
				
				//theStep->GetTrack()->SetMomentumDirection(MomentumDirection_i); // I see strange result and don't know why
				//cout << "InHole !" << "x_center = " << x_center << " \t y_center = " << y_center << endl;
				//cout << "x - x_center = " << x - x_center << "\t y - y_center = " << y - y_center << endl;
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
			//theStep->GetTrack()->SetTrackStatus(fStopAndKill);

		#ifndef Cu_REFLECTION
			theStep->GetTrack()->SetTrackStatus(fStopAndKill);
		#endif //Cu_REFLECTION
		}



	}

}
#endif // bPASSTHROUGHTHGEMsREFLECTION