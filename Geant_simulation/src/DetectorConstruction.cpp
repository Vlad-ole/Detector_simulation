#include "DetectorConstruction.hh"
#include "Messenger.hh" 
#include "CathodeSD.hh"
#include "PrimaryGeneratorAction.hh"

#include <string>
#include <sstream>
#include <limits>

#include <G4Material.hh>
#include <G4Box.hh>
#include <G4Trap.hh>
#include <G4Tubs.hh>
#include <G4Cons.hh>
#include <G4Ellipsoid.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4PVParameterised.hh>
#include <G4SDManager.hh>
#include <G4GeometryTolerance.hh>
#include <G4GeometryManager.hh>
#include <G4NistManager.hh>
#include <G4RunManager.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4SolidStore.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4SurfaceProperty.hh>
#include <G4UImanager.hh>
#include <G4OpticalSurface.hh>
#include <G4UserLimits.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4ios.hh>
#include <G4GeneralParticleSource.hh>
#include <G4RegionStore.hh>
#include <G4Trd.hh>

#include <G4SystemOfUnits.hh> // this has appeared in GEANT4_10
#include <string>


using namespace std;

//#define CHAMFER //It is not so important



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
	messenger = new Messenger(this); 
}

DetectorConstruction::~DetectorConstruction()
{
	delete messenger; 
}


// это основной метод
G4VPhysicalVolume * DetectorConstruction::Construct()
{
	G4UImanager* UI = G4UImanager::GetUIpointer();

	defineMaterials(); // внешн€€ функци€. «десь определ€ютс€ различные материалы.
	defineSurfaces(); // внешн€€ функци€. «десь определ€ютс€ различные типы поверхностей.

	//--------------------------------------------------------------------------------
	//выставление размеров объектов
	G4double HalfWorldLength = 10*cm;

	const G4double alpha = 0 * degree;
	const double scintillator_length_x = 3.5*mm; // full length (or the narrower side)	
	const double scintillator_height = 8.95*mm; // full length

	const double scintillator_length_y = 3.5; // full length (or the wider side)
	//double scintillator_height = 2*mm + 0.5*mm; // for YAP:Ce 2x10 only

	cout << "scintillator_length_y = scintillator_height*tan(alpha) + 3.5 = " << scintillator_length_y << endl;
	
	const double grease_diameter = 1.5*max(scintillator_length_x, scintillator_length_y);
	const double grease_height = 0.1*mm  /*0.0*mm*/;

	const double absorber_diameter = grease_diameter;
	const double absorber_height = 1 * um;

	const double glass_diameter = 5 * cm;
	const double glass_height = 2 * mm;

	const double cathode_diameter = glass_diameter;
	const double cathode_height = 1 * um;




	const double thickness_SiPM = 1*nm;
	const double size_SiPM = 6 * mm;








	G4int NbOfChambers = 5;
	G4double chamberSpacing = 10 * mm; // from chamber center to center!

	G4double chamberWidth = 6 * mm; // width of the chambers
	G4double targetLength = 5.0*mm; // full length of Target

	G4double trackerLength = (NbOfChambers + 1)*chamberSpacing;

	G4double worldLength = 1.2 * (2 * targetLength + trackerLength);

	G4double targetRadius = 0.5*targetLength;   // Radius of Target
	targetLength = 0.5*targetLength;             // Half length of the Target  
	G4double trackerSize = 0.5*trackerLength;  // Half length of the Tracker

	G4double firstPosition = -trackerSize + chamberSpacing;
	G4double firstLength = trackerLength / 10;
	G4double lastLength = trackerLength;

	//--------------------------------------------------------------------------------






	//--------------------------------------------------------------------------------
	//определение взаимного расположени€ объектов

	//const G4double shift = scintillator_height / 2.0 + scintillator_length_x / 4.0 * tan(alpha);
	const G4double shift = scintillator_height / 2.0;


	const G4ThreeVector &position_SiPM = G4ThreeVector(0, 0, 79.2*mm + thickness_SiPM / 2.0);


	const G4ThreeVector &scintillator_position = G4ThreeVector(0, 0, shift);
	//const G4ThreeVector &absorber_position = G4ThreeVector(0, 0, scintillator_height * 0.99);
	const G4ThreeVector &grease_position = G4ThreeVector(0, 0, 2*shift + grease_height / 2.0);
	const G4ThreeVector &glass_position = G4ThreeVector(0, 0, 2*shift + grease_height + glass_height / 2.0);
	const G4ThreeVector &cathode_position = G4ThreeVector(0, 0, 2*shift + grease_height + glass_height + cathode_height / 2.0);


	//--------------------------------------------------------------------------------





	//----------------------------------------------------------------------------------
	// создание мира
	solidWorld= new G4Box("sworld", HalfWorldLength, HalfWorldLength, HalfWorldLength);
	logicWorld= new G4LogicalVolume( solidWorld, G4Material::GetMaterial("Air"), "lWorld", 0, 0, 0);

	//  Must place the World Physical volume unrotated at (0,0,0).
	//
	physiWorld = new G4PVPlacement(0,               // no rotation
		G4ThreeVector(), // at (0,0,0)
		logicWorld,      // its logical volume
		"pWorld",         // its name
		0,               // its mother  volume
		false,           // no boolean operations
		0);              // copy number

	// Set user cuts to avoid deadlocks
	G4double maxStep = 10.0*m, maxLength = 10.0*m, maxTime = 100.0*ns, minEkin = 0.5*eV;
	logicWorld->SetUserLimits(new G4UserLimits(maxStep,maxLength,maxTime,minEkin));
	//-------------------------------------------------------------------------------



	//--------------------------------------------------------------------------------
	// create SiPM matrix

	int Nx_SiPMs = 11;
	int Ny_SiPMs = 11;

	solid_SiPM = new G4Box("sscintillator", size_SiPM / 2.0, size_SiPM / 2.0, thickness_SiPM / 2.0);
	logic_SiPM = new G4LogicalVolume(solid_SiPM, G4Material::GetMaterial("Air"), "lSiPM", 0, 0, 0);
	//physi_SiPM = new G4PVPlacement(0,
	//	position_SiPM,  // at (x,y,z)
	//	logic_SiPM,     // its logical volume
	//	"pSiPM",        // its name
	//	logicWorld,      // its mother  volume
	//	false,           // no boolean operations
	//	0);

	G4ThreeVector positionTracker = G4ThreeVector(0, 0, 0);

	G4Box* trackerS
		= new G4Box("tracker", 9*cm, 9 * cm, 9 * cm);
	G4LogicalVolume* trackerLV
		= new G4LogicalVolume(trackerS, G4Material::GetMaterial("Air"), "Tracker", 0, 0, 0);
	G4VPhysicalVolume* phys_tracker = new G4PVPlacement(0,               // no rotation
		positionTracker, // at (x,y,z)
		trackerLV,       // its logical volume
		"Tracker",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 



	G4VPVParameterisation* chamberParam =
		new DetectorParametrisation(Nx_SiPMs, Ny_SiPMs, 0, 0, 79.2*mm, 10*mm, 6*mm);
			//NbOfChambers,   // NoChambers
			//firstPosition,  // Z of center of first
			//chamberSpacing, // Z spacing of centers
			//chamberWidth,  // chamber width
			//firstLength,    // initial length 
			//lastLength);    // final length

							// dummy value : kZAxis -- modified by parameterised volume

	G4VPhysicalVolume* phys_SiPM = new G4PVParameterised("phys_SiPM",       // their name
		logic_SiPM,   // their logical volume
		trackerLV,       // Mother logical volume
		kXAxis,          // Are placed along this axis 
		Nx_SiPMs * Ny_SiPMs,    // Number of chambers
		chamberParam,    // The parametrisation
		fCheckOverlaps); // checking overlaps 

	//G4cout << "There are " << NbOfChambers << " chambers in the tracker region. "
	//	<< G4endl
	//	<< "The chambers are " << chamberWidth / cm << " cm of "
	//	<< fChamberMaterial->GetName() << G4endl
	//	<< "The distance between chamber is " << chamberSpacing / cm << " cm"
	//	<< G4endl;


	//--------------------------------------------------------------------------------










	//////--------------------------------------------------------------------------------
	////// создание кристалла



	//////// for any crystall
	//solid_scintillator = new G4Box("sscintillator", scintillator_length_x/2.0, scintillator_length_y/2.0, scintillator_height/2.0);
	//G4RotationMatrix* yRot90deg = new G4RotationMatrix;
	//
	//logicScint = new G4LogicalVolume(solid_scintillator, G4Material::GetMaterial("LFS-3"), "lScintillator",0,0,0);

	//physiScint = new G4PVPlacement(yRot90deg,               
	//	scintillator_position,  // at (x,y,z)
	//	logicScint,     // its logical volume
	//	"pScintillator",        // its name
	//	logicWorld,      // its mother  volume
	//	false,           // no boolean operations
	//	0); 

	////--------------------------------------------------------------------------------






	//////-----------------------------------------------------------------------------
	////создание фотокатода
	//solidCathode = new G4Tubs("scath", 0.*cm, cathode_diameter/2.0 , cathode_height/2.0, 0.*deg, 360.*deg);
	//logicCathode = new G4LogicalVolume(solidCathode, G4Material::GetMaterial("BialkaliCathode"), "BialkaliCathode_", 0,0,0);
	//physiCathode = new G4PVPlacement(0,               // no rotation
	//	cathode_position,  // at (x,y,z)
	//	logicCathode,     // its logical volume 
	//	"pCath",          // its name
	//	logicWorld,       // its mother  volume
	//	false,            // no boolean operations
	//	0);               // copy number


	//////--------------------------------------------------------------------------




	//определение чувствительного объема
	G4String sensitiveDetectorName = "/detector/sensitiveDetector";
	theCathodeSD = new CathodeSD(sensitiveDetectorName, phys_SiPM, Nx_SiPMs * Ny_SiPMs);
	G4SDManager::GetSDMpointer()->AddNewDetector( theCathodeSD );
	logic_SiPM->SetSensitiveDetector( theCathodeSD );
	////------------------------------------------------------------------------


	//---------------------------------------------------------------------------
	//установка поверхностей
	//G4LogicalBorderSurface* scintillator_world_logical = new G4LogicalBorderSurface("world_scintillator", physiScint, physiWorld, teflon_unified); // from physiScint to physiWorld
	//G4LogicalBorderSurface* world_scintillator_logical = new G4LogicalBorderSurface("scintillator_world", physiWorld, physiScint, teflon_unified); // from physiWorld to physiScint

	//G4LogicalBorderSurface* scintillator_grease_logical = new G4LogicalBorderSurface("scintillator_world", physiScint, physi_grease, polishedAir);
	//G4LogicalBorderSurface* grease_scintillator_logical = new G4LogicalBorderSurface("scintillator_world", physi_grease, physiScint, polishedAir);

	//G4LogicalBorderSurface* grease_glass_logical = new G4LogicalBorderSurface("grease_glass_logical", physi_grease, physi_glass, polishedAir);
	//G4LogicalBorderSurface* glass_grease_logical = new G4LogicalBorderSurface("glass_grease_logical", physi_glass, physi_grease, polishedAir);

	//G4LogicalBorderSurface* envelope2CathodeSurface = new G4LogicalBorderSurface("envelope2CathodeSurface", physi_glass, physiCathode, silicaCathodeMaterial);
	G4LogicalBorderSurface* tracker2SiPM = new G4LogicalBorderSurface("envelope2CathodeSurface", phys_tracker, phys_SiPM, silicaCathodeMaterial);


	
	//---------------------------------------------------------------------------


	//------------------------------------------------------------------------------
	// установка атрибутов визуализации
	G4VisAttributes* ScintVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,1.0, 0.6));

	G4VisAttributes* CathodeVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0, 0.5));
	CathodeVisAtt->SetForceWireframe(true);

	G4VisAttributes* GreaseVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0,0.8));

	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	trackerLV->SetVisAttributes(G4VisAttributes::Invisible);
//	logicScint->SetVisAttributes(ScintVisAtt);
	//logic_grease->SetVisAttributes(GreaseVisAtt);
	//logicCathode->SetVisAttributes(CathodeVisAtt);
	//-----------------------------------------------------------------------------

	return physiWorld;
}


void DetectorConstruction::ChangeDetectorConstruction(double parametr)
{
	ChangeSurface(parametr);
	//ChangeMaterials();	
}

