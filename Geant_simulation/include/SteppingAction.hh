#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "PMTHits.h"
#include <G4ThreeVector.hh>

class DetectorConstruction;
class EventAction;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(DetectorConstruction* myDC, EventAction* myEA);
    virtual ~SteppingAction(){ delete pmt_hits; };

    virtual void UserSteppingAction(const G4Step*);

	PMTHits* pmt_hits;

  private:
    DetectorConstruction* myDetector;
    EventAction* eventAction;
	void PassThroughGEM(const G4Step*, G4double z_pos, G4double z_size);
	void ReturnFromSingleTHGEMHole(const G4Step* theStep);
	double delta_z;
	bool is_photon_inside_single_THGEM_hole;
	G4String Volume_init_THGEM;
	G4ThreeVector MomentumDirection_init_THGEM;
	G4ThreeVector Position_init_THGEM;
	G4ThreeVector Position_init_SingleTHGEMHole;
	G4ThreeVector Position_final_SingleTHGEMHole;
	
};

#endif
