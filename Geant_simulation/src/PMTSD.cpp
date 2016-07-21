#include "PMTSD.h"

#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4ThreeVector.hh>
#include <G4Track.hh>
#include <G4DynamicParticle.hh>
#include <G4SDManager.hh>
#include <G4ios.hh>
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"


#include "Singleton.h"

#include <iostream>
#include <fstream>

#define DEBAG_MODE

PMTSD::PMTSD(G4String name, G4VPhysicalVolume *vol, int N_SiPMs) : G4VSensitiveDetector(name), _vol(vol)
{

}

PMTSD::~PMTSD()
{
	double x_pos;
	double y_pos;
	double step = 10;

	const double x_pos_PMT = 152 / 2.0;
	const double y_pos_PMT = x_pos_PMT;

	g()->file_num_of_photons_PMT << 0 << "\t" << -x_pos << "\t" << 0 << "\t" << N_reg_number[0] << endl;
	g()->file_num_of_photons_PMT << 1 << "\t" << x_pos << "\t" << 0 << "\t" << N_reg_number[1] << endl;
	g()->file_num_of_photons_PMT << 2 << "\t" << 0 << "\t" << -y_pos_PMT << "\t" << N_reg_number[2] << endl;
	g()->file_num_of_photons_PMT << 3 << "\t" << 0 << "\t" << y_pos_PMT << "\t" << N_reg_number[3] << endl;
	
}

void PMTSD::Initialize(G4HCofThisEvent* HCE)
{

}

G4bool PMTSD::ProcessHits(G4Step*, G4TouchableHistory*)
{
	return true;
}

//Generates a hit and uses the postStepPoint's mother volume replica number
//PostStepPoint because the hit is generated manually when the photon is
//absorbed by the photocathode

G4bool PMTSD::ProcessHits_Optical(const G4Step* aStep, G4TouchableHistory*, int N_detector)
{

	//need to know if this is an optical photon
	if (aStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;


	if (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_PMT0")
		N_reg_number[0]++;

	if (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_PMT1")
		N_reg_number[1]++;

	if (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_PMT2")
		N_reg_number[2]++;

	if (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "phys_PMT3")
		N_reg_number[3]++;



	return true;
}

void PMTSD::EndOfEvent(G4HCofThisEvent*)
{
	//.. print info about collection of hits

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
