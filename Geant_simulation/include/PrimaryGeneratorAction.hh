#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include <vector>

//root cern
#include "TGraph.h"

#define  MAX_SPECTER_DATALINES  150

enum XRAY_TYPE
{
	MONO,
	SPECTER_continuous,
	SPECTER_discrete
};


class G4ParticleGun;
class G4Event;


class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();              //mono
	PrimaryGeneratorAction(const char* fname, bool IsDiscrete);   //specter continuous
   ~PrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event* anEvent);

  private:
    void CommonPart();
    G4ParticleGun* particleGun;
	
	XRAY_TYPE xrType;
    //int       nMaxDataLines;
    //double    dEnergy[MAX_SPECTER_DATALINES];
    //double    dWeight[MAX_SPECTER_DATALINES];
	vector<double> dEnergy;
	vector<double> dWeight;
	double Energy_min;
	double Energy_max;
    double dMaxWeight;
	TGraph *gr_energy_weight;
	vector<double> Ev;
	vector<double> pv;
	vector<double> pv_edge;//right edge

	bool bOldHole;
	int particle_counter;
	double x_shift;
	double y_shift;
};

#endif


