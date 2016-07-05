#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "PMTHits.h"

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

	

};

#endif
