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

#include "AnodeGridParametrisation.h"


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
	const G4double HalfWorldLength = 50 * cm;

	//SiPMs
	const int Nx_SiPMs = 11;
	const int Ny_SiPMs = 11;

	const double thickness_SiPM = 1 * nm;
	const double size_SiPM = 6 * mm;
	const double chamberSpacing = 10 * mm;
	const double z_SiPM_bottom = 79.2*mm;
	const double z_SiPM_center = z_SiPM_bottom + thickness_SiPM / 2.0;

	//anode wire
	const double radius_wire = 150 * um;
	const double length_wire = 60 * mm;
	const double step_wire = 1 * mm;
	const int N_wire = 59;

	//Anode_grid
	const double thickness_anode_grid = 0.5 * mm;
	const double size_anode_grid = 124 * mm;
	const double size_anode_grid_hole = 60 * mm;
	const double z_anode_grid_bottom = 76.2*mm;
	const double z_anode_grid_center = z_anode_grid_bottom + thickness_anode_grid / 2.0;

	//tracker SiPM
	const double x_size_tracker = Nx_SiPMs * chamberSpacing + size_SiPM / 2.0;
	const double y_size_tracker = Ny_SiPMs * chamberSpacing + size_SiPM / 2.0;
	const double z_size_tracker = 0.1 * mm;

	//tracker Anode_grid
	const double x_size_tracker_anode_grid = size_anode_grid;
	const double y_size_tracker_anode_grid = size_anode_grid;
	const double z_size_tracker_anode_grid = thickness_anode_grid;

	//PMMA plate
	const double x_size_PMMA_plate = size_anode_grid;
	const double y_size_PMMA_plate = size_anode_grid;
	const double z_size_PMMA_plate = 1.5 * mm;
	const double z_PMMA_plate_center = z_anode_grid_center + thickness_anode_grid / 2.0 + z_size_PMMA_plate / 2.0;


	if (thickness_anode_grid < 2 * radius_wire)
	{
		cout << "error: thickness_anode_grid < 2*radius_wire" << endl;
		system("pause");
	}



	//--------------------------------------------------------------------------------






	//--------------------------------------------------------------------------------
	//определение взаимного расположени€ объектов

	const G4ThreeVector &position_SiPM = G4ThreeVector(0, 0, 0);
	const G4ThreeVector &position_anode_grid = G4ThreeVector(0, 0, z_anode_grid_center);
	const G4ThreeVector &positionTracker = G4ThreeVector(0, 0, z_SiPM_center);
	const G4ThreeVector &position_PMMA_plate = G4ThreeVector(0, 0, z_PMMA_plate_center);
	//-------------------------------------------------------------------------------





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
	// create tracker (this need for SiPM_matrix parametrising)

	G4Box* trackerS
		= new G4Box("tracker", x_size_tracker / 2.0, y_size_tracker / 2.0, z_size_tracker / 2.0);
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

	//--------------------------------------------------------------------------------





	//--------------------------------------------------------------------------------
	// create tracker_anode_grid (this need for anode_grid parametrising)

	G4Box* solid_tracker_anode_grid = new G4Box("solid_tracker_anode_grid", x_size_tracker_anode_grid / 2.0, y_size_tracker_anode_grid / 2.0, z_size_tracker_anode_grid / 2.0);
	G4LogicalVolume* logic_tracker_anode_grid = new G4LogicalVolume(solid_tracker_anode_grid, G4Material::GetMaterial("Air"), "logic_tracker_anode_grid", 0, 0, 0);
	G4VPhysicalVolume* phys_tracker_anode_grid = new G4PVPlacement(0,               // no rotation
		position_anode_grid, // at (x,y,z)
		logic_tracker_anode_grid,       // its logical volume
		"phys_tracker_anode_grid",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	//--------------------------------------------------------------------------------





	//--------------------------------------------------------------------------------
	// create SiPM matrix



	solid_SiPM = new G4Box("sscintillator", size_SiPM / 2.0, size_SiPM / 2.0, thickness_SiPM / 2.0);
	logic_SiPM = new G4LogicalVolume(solid_SiPM, G4Material::GetMaterial("Air"), "lSiPM", 0, 0, 0);
	//physi_SiPM = new G4PVPlacement(0,
	//	position_SiPM,  // at (x,y,z)
	//	logic_SiPM,     // its logical volume
	//	"pSiPM",        // its name
	//	logicWorld,      // its mother  volume
	//	false,           // no boolean operations
	//	0);
	

	G4VPVParameterisation* chamberParam =
		new DetectorParametrisation(Nx_SiPMs, Ny_SiPMs, 0, 0, 0, chamberSpacing, size_SiPM);
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




	G4double a;  // atomic mass
	G4double z;  // atomic number
	G4double density;
	G4Material* fAl = new G4Material("Al", z = 13., a = 26.98*g / mole, density = 2.7*g / cm3);


	//--------------------------------------------------------------------------------
	//create anode grid
	G4Box* solid_anode_grid_substrate = new G4Box("anode_grid_substrate", size_anode_grid / 2.0, size_anode_grid / 2.0, thickness_anode_grid / 2.0);
	G4Box* solid_anode_grid_hole = new G4Box("anode_grid_hole", size_anode_grid_hole / 2.0, size_anode_grid_hole / 2.0, thickness_anode_grid * 0.52);

	G4SubtractionSolid* solid_anode_grid_subtraction = new G4SubtractionSolid("anode_grid__substrate-hole", solid_anode_grid_substrate, solid_anode_grid_hole);

	logic_anode_grid = new G4LogicalVolume(solid_anode_grid_subtraction, fAl, "l_anode_grid", 0, 0, 0);
	phys_anode_grid = new G4PVPlacement(0,
		position_anode_grid,  // at (x,y,z)
		logic_anode_grid,     // its logical volume
		"p_anode_grid",        // its name
		logicWorld,      // its mother  volume
		false,           // no boolean operations
		0); 

	//--------------------------------------------------------------------------------







	//--------------------------------------------------------------------------------
	//create anode wire
	G4Tubs* solid_wire = new G4Tubs("solid_wire", 0, radius_wire, length_wire / 2.0, 0.*deg, 360.*deg);
	G4LogicalVolume* logic_wire = new G4LogicalVolume(solid_wire, fAl, "lwire", 0, 0, 0);
	G4VPVParameterisation* param_wire = new AnodeGridParametrisation(N_wire, 0, 0, 0, step_wire, radius_wire, length_wire);

	G4VPhysicalVolume* phys_wire = new G4PVParameterised("phys_wire",       // their name
		logic_wire,   // their logical volume
		logic_tracker_anode_grid,       // Mother logical volume
		kXAxis,          // Are placed along this axis 
		N_wire,    // Number of chambers
		param_wire,    // The parametrisation
		fCheckOverlaps); // checking overlaps 


	//--------------------------------------------------------------------------------








	//--------------------------------------------------------------------------------
	//create PMMA plate
	G4Box* solid_PMMA_plate = new G4Box("solid_tracker_anode_grid", x_size_PMMA_plate / 2.0, y_size_PMMA_plate / 2.0, z_size_PMMA_plate / 2.0);
	G4LogicalVolume* logic_PMMA_plate = new G4LogicalVolume(solid_PMMA_plate, G4Material::GetMaterial("PMMA"), "logic_PMMA_plate", 0, 0, 0);
	G4VPhysicalVolume* phys_PMMA_plate = new G4PVPlacement(0,               // no rotation
		position_PMMA_plate, // at (x,y,z)
		logic_PMMA_plate,       // its logical volume
		"phys_PMMA_plate",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 


	//--------------------------------------------------------------------------------




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
	
	G4LogicalBorderSurface* tracker2SiPM = new G4LogicalBorderSurface("tracker2SiPM", phys_tracker, phys_SiPM, silicaCathodeMaterial);
	//G4LogicalBorderSurface* World2Traker = new G4LogicalBorderSurface("World2Traker", physiWorld, phys_tracker, AbsorberMaterial);
	//G4LogicalBorderSurface* Traker2World = new G4LogicalBorderSurface("Traker2World", phys_tracker, physiWorld, AbsorberMaterial);

	
	//---------------------------------------------------------------------------


	//------------------------------------------------------------------------------
	// установка атрибутов визуализации
	G4VisAttributes* ScintVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,1.0, 0.6));

	G4VisAttributes* CathodeVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0, 0.5));
	CathodeVisAtt->SetForceWireframe(true);

	G4VisAttributes* GreaseVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0,0.8));
	logic_anode_grid->SetVisAttributes(GreaseVisAtt);

	G4VisAttributes* WireVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.8));
	logic_wire->SetVisAttributes(WireVisAtt);

	G4VisAttributes* AnodeGridVisAtt = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0, 0.8));
//	logic_anode_grid->SetVisAttributes(AnodeGridVisAtt);

	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
//	trackerLV->SetVisAttributes(G4VisAttributes::Invisible);
	//logic_SiPM->SetVisAttributes(G4VisAttributes::Invisible);
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

