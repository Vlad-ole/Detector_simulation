#include "DetectorParametrisation.h"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorParametrisation::DetectorParametrisation(G4int noChambersX, G4int    noChambersY, G4double startX, G4double startY, G4double startZ, G4double spacing, G4double widthChamber)
	: G4VPVParameterisation()
{
	fNoChambersX = noChambersX;
	fNoChambersY = noChambersY;
	fStartX = startX;
	fStartY = startY;
	fStartZ = startZ;
	fHalfWidth = 0.5*widthChamber;
	fSpacing = spacing;

	yCol = 0;

	if (noChambersX > 0 && noChambersY > 0 && spacing < widthChamber)
	{
		G4Exception("DetectorParametrisation::DetectorParametrisation()",
			"InvalidSetup", FatalException,
			"Width>Spacing");

	}
}

DetectorParametrisation::~DetectorParametrisation()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorParametrisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol)  const
{
	// Note: copyNo will start with zero!
	G4double Xposition = fStartX + (-fNoChambersX / 2 + copyNo % fNoChambersX) * fSpacing;
	G4double Yposition = fStartY + (-fNoChambersY / 2 + copyNo / fNoChambersX) * fSpacing;

	//G4cout << "copyNo = " << copyNo << "; Xposition = " << Xposition << "; Yposition = " << Yposition << G4endl;

	G4ThreeVector origin(Xposition, Yposition, fStartZ);
	physVol->SetTranslation(origin);
	physVol->SetRotation(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorParametrisation::ComputeDimensions
(G4Box& trackerChamber, const G4int copyNo, const G4VPhysicalVolume*) const
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
