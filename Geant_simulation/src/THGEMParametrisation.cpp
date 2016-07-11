#include "THGEMParametrisation.h"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

THGEMParametrisation::THGEMParametrisation(G4int  Nx_cells_THGEM2, G4int Ny_cells_THGEM2, G4double startX, G4double startY, G4double startZ, G4double step_THGEM)
{
	fNx = Nx_cells_THGEM2;
	fNy = Ny_cells_THGEM2;
	fStartX = startX;
	fStartY = startY;
	fStartZ = startZ;
	fSpacing = step_THGEM;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

THGEMParametrisation::~THGEMParametrisation()
{
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void THGEMParametrisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol)  const
{

	// Note: copyNo will start with zero!
	
	G4double Xposition = fStartX + (-fNx / 2 + copyNo % fNx) * fSpacing;
	if ( (copyNo / fNx) % 2 == 0 )
	{
		Xposition -= fSpacing / 2.0;
		//G4cout << G4endl << copyNo << "\t" << copyNo / fNx << "\t" << (copyNo / fNx) % 2 << "\t" << ((copyNo / fNx) % 2 == 0) << G4endl;
	}
	
	G4double Yposition = fStartY + (-fNy / 2 + copyNo / fNx) * (fSpacing * sqrt(3) / 2 );

	//G4cout << "copyNo = " << copyNo << "; Xposition = " << Xposition << "; Yposition = " << Yposition << G4endl;

	G4ThreeVector origin(Xposition, Yposition, fStartZ);
	physVol->SetTranslation(origin);
	physVol->SetRotation(0);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void THGEMParametrisation::ComputeDimensions
(G4Box& trackerChamber, const G4int copyNo, const G4VPhysicalVolume*) const
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
