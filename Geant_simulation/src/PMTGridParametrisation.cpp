#include "PMTGridParametrisation.h"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PMTGridParametrisation::PMTGridParametrisation
(G4int  NoObj, G4double startX, G4double startY, G4double startZ, G4double spacing, G4double radius, G4double length)
{
	fNoObj = NoObj;
	fStartX = startX;
	fStartY = startY;
	fStartZ = startZ;
	fSpacing = spacing;
	fRadius = radius;
	fLength = length;

	rotX = new G4RotationMatrix();
	rotX->rotateX(90 * deg);

	if (fNoObj > 0 && spacing < 2 * radius)
	{
		G4Exception("DetectorParametrisation::DetectorParametrisation()",
			"InvalidSetup", FatalException,
			"Width>Spacing");

	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PMTGridParametrisation::~PMTGridParametrisation()
{
	delete rotX;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PMTGridParametrisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol)  const
{
	// Note: copyNo will start with zero!
	G4double Xposition = fStartX + (-fNoObj / 2 + copyNo) * fSpacing;

	//G4cout << "copyNo = " << copyNo << "; Xposition = " << Xposition << "; Yposition = " << Yposition << G4endl;

	G4ThreeVector origin(Xposition, fStartY, fStartZ);

	physVol->SetTranslation(origin);
	physVol->SetRotation(rotX);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PMTGridParametrisation::ComputeDimensions
(G4Box& trackerChamber, const G4int copyNo, const G4VPhysicalVolume*) const
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
