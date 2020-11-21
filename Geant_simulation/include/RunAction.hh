#ifndef RunAction_h
#define RunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include "Hits.h"

class G4Timer;
class G4Run;

class RunAction : public G4UserRunAction
{
  public:
     RunAction();
    ~RunAction();

  public:
    void BeginOfRunAction(const G4Run* aRun);
    void EndOfRunAction(const G4Run* aRun);
	void Print(Hits* hits, std::ofstream& file);

  private:
    G4Timer* timer;
	Hits* SiPM_hits_all_runs;
	Hits* SiPM_hits_all_runs_LRF;
	int N_runs_used_for_LRF;
};

#endif

