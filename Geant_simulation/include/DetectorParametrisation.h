#ifndef DetectorParametrisation_h
#define DetectorParametrisation_h 1

#include "G4VPVParameterisation.hh"

class G4VPhysicalVolume;
class G4Box;

// Dummy declarations to get rid of warnings ...
class G4Trd;
class G4Trap;
class G4Cons;
class G4Orb;
class G4Sphere;
class G4Ellipsoid;
class G4Torus;
class G4Para;
class G4Hype;
class G4Tubs;
class G4Polycone;
class G4Polyhedra;

///  A parameterisation that describes a series of boxes along Z.
///
///  The boxes have equal width, & their lengths are a linear equation.
///  They are spaced an equal distance apart, starting from given location.

class DetectorParametrisation : public G4VPVParameterisation
{
public:

	DetectorParametrisation(G4int    noChambersX, G4int    noChambersY,
		G4double startX,
		G4double startY,
		G4double startZ,
		G4double spacing,
		G4double widthChamber);

	virtual ~DetectorParametrisation();

	void ComputeTransformation(const G4int copyNo,
		G4VPhysicalVolume* physVol) const;

	void ComputeDimensions(G4Box & trackerLayer, const G4int copyNo,
		const G4VPhysicalVolume* physVol) const;

private:  // Dummy declarations to get rid of warnings ...

	void ComputeDimensions(G4Tubs&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Trd&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Trap&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Cons&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Sphere&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Orb&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Ellipsoid&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Torus&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Para&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Hype&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Polycone&, const G4int,
		const G4VPhysicalVolume*) const {}
	void ComputeDimensions(G4Polyhedra&, const G4int,
		const G4VPhysicalVolume*) const {}

private:

	G4int    fNoChambersX;
	G4int    fNoChambersY;

	G4double fStartX;
	G4double fStartY;
	G4double fStartZ;

	G4double fHalfWidth;        //  The half-width of each tracker chamber
	G4double fSpacing;          //  The distance between the box' center

	mutable int yCol;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
