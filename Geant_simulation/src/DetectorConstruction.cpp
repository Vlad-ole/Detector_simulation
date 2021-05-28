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
#include "PMTGridParametrisation.h"
#include "THGEMParametrisation.h"
#include "PMTSD.h"
#include "Singleton.h"

using namespace std;

//#define CHAMFER //It is not so important



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// это основной метод
G4VPhysicalVolume * DetectorConstruction::Construct()
{
	//G4UImanager* UI = G4UImanager::GetUIpointer();

	
//#define SETUP1 //(TPB, w/o alpha, THGEM 28%)
//#define SETUP2 // (w/o TPB, alpha, THGEM 75%, UV acrylic 4mm)
#define SETUP3 // (w/o TPB, with or w/o alpha, THGEM 75%, UV acrylic 4mm)

#ifdef SETUP3
	#define bSiPM
	#define bPMMA_plate 
	#define bAnode_grid
	#define bInsulator_box
	#define bPMTs
	#define bPMTAnodeGrid
	#define bSteelBox
	#define bTHGEM1
	#define bTHGEM0
	#define bFieldTHGEM
	//#define bFieldWires
	#define bLArOuter 
	#define bLArInner
	//#define bAlpha
	#define bCathode
#endif

	

//z >= 0
//#define bSiPM
//#define bPMMA_plate 
//#define bAnode_grid
//#define bInsulator_box
//#define bPMTs
////#define bWLS
////#define bTHGEM2
//#define bTHGEM1
//#define bTHGEM0
////#define bCuReflector
////#define bSingleTHGEMHole
////#define bFieldTHGEM
//#define bFieldWires
//#define	bLArOuter 
//#define bLArInner
//#define bCathode

//z < 0
//#define bLArInactive
//#define bPMMA_bottom
//#define bAl_window
//#define bCryogenicChamberBottom
//#define bExternalColl6mm
//#define bExternalColl2mm
//#define bCd109ExternalBox
//#define bCd109InternalColl
//#define bCd109IsotopBoxHolder
//#define bCd109
//#define bCd109BeFoil
//#define bCd109WolframPlug
//#define bCd109CuFoil
//#define bCd109Detector

	//bool bLAr_inner = false;
	//bool bLAr_outer = false;


	defineMaterials();
	defineSurfaces(); 
	SetSizeAndPosition();		

#ifdef bExternalColl6mm
	const double diameter_inter_ExternalColl = 6 * mm;
#endif // bExternalColl6mm
#ifdef bExternalColl2mm
	const double diameter_inter_ExternalColl = 2 * mm;
#endif // bExternalColl2mm
	
	G4RotationMatrix* rotX = new G4RotationMatrix();
	rotX->rotateX(90 * deg);

	G4RotationMatrix* rotY = new G4RotationMatrix();
	rotY->rotateY(90 * deg);

	G4RotationMatrix* rotZ = new G4RotationMatrix();
	rotZ->rotateZ(90 * deg);
	//-------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------
	// создание мира
	solidWorld = new G4Box("sworld", HalfWorldLength, HalfWorldLength, HalfWorldLength);
	logicWorld = new G4LogicalVolume(solidWorld, G4Material::GetMaterial("Air"), "lWorld", 0, 0, 0);

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
	logicWorld->SetUserLimits(new G4UserLimits(maxStep, maxLength, maxTime, minEkin));
	//-------------------------------------------------------------------------------


	//-------------------------------------------------------------------------------
	//create LAr box contaned inside the PMMA insulator

	G4Box* solid_LAr_inner
		= new G4Box("solid_LAr_inner", x_size_LAr_inner / 2.0, y_size_LAr_inner / 2.0, z_size_LAr_inner / 2.0);
	G4LogicalVolume* logic_LAr_inner
		= new G4LogicalVolume(solid_LAr_inner, G4Material::GetMaterial("LAr"), "logic_LAr_inner", 0, 0, 0);
#ifdef bLArInner
	G4VPhysicalVolume* phys_LAr_inner = new G4PVPlacement(0,               // no rotation
		position_LAr_inner, // at (x,y,z)
		logic_LAr_inner,       // its logical volume
		"phys_LAr_inner",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
	//-------------------------------------------------------------------------------
#endif //bLArInner





	//--------------------------------------------------------------------------------
	//create LAr box contaned outside the PMMA insulator
	cout << x_size_LAr_outer_out / 2.0 << "\t" << y_size_LAr_outer_out / 2.0 << endl;
	G4Box* solid_LAr_outer_out = new G4Box("solid_LAr_outer_out", x_size_LAr_outer_out / 2.0, y_size_LAr_outer_out / 2.0, z_size_LAr_outer / 2.0);
	G4Box* solid_LAr_outer_in = new G4Box("solid_LAr_outer_in", x_size_LAr_outer_in / 2.0, y_size_LAr_outer_in / 2.0, z_size_LAr_outer / 2.0);
	G4SubtractionSolid* solid_LAr_outer = new G4SubtractionSolid("solid_LAr_outer", solid_LAr_outer_out, solid_LAr_outer_in);

	G4LogicalVolume* logic_LAr_outer;
#ifdef bLArOuter
	logic_LAr_outer = new G4LogicalVolume(solid_LAr_outer, G4Material::GetMaterial("LAr"), "logic_LAr_outer", 0, 0, 0);
#else
	logic_LAr_outer = new G4LogicalVolume(solid_LAr_outer, G4Material::GetMaterial("Air"), "logic_LAr_outer", 0, 0, 0);
#endif // bLArOuter
	
	G4VPhysicalVolume* phys_LAr_outer = new G4PVPlacement(0,               // no rotation
		position_LAr_outer, // at (x,y,z)
		logic_LAr_outer,       // its logical volume
		"phys_LAr_outer",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	//--------------------------------------------------------------------------------





	//--------------------------------------------------------------------------------
	//create FieldWires
#ifdef	bFieldWires
	G4Tubs* solid_FieldWire = new G4Tubs("solid_FieldWire", 0, radius_FieldWire, length_FieldWire / 2.0, 0.*deg, 360.*deg);
	G4LogicalVolume* logic_FieldWire = new G4LogicalVolume(solid_FieldWire, G4Material::GetMaterial("Al"), "logic_FieldWire", 0, 0, 0);
	
	G4VPhysicalVolume* phys_FieldWire_bottom1 = new G4PVPlacement(rotX,               // no rotation
		position_FieldWire_bottom1, // at (x,y,z)
		logic_FieldWire,       // its logical volume
		"phys__FieldWire_bottom1",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	G4VPhysicalVolume* phys_FieldWire_bottom2 = new G4PVPlacement(rotX,               // no rotation
		position_FieldWire_bottom2, // at (x,y,z)
		logic_FieldWire,       // its logical volume
		"phys__FieldWire_bottom2",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	G4VPhysicalVolume* phys_FieldWire_bottom3 = new G4PVPlacement(rotY,               // no rotation
		position_FieldWire_bottom3, // at (x,y,z)
		logic_FieldWire,       // its logical volume
		"phys__FieldWire_bottom3",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	G4VPhysicalVolume* phys_FieldWire_bottom4 = new G4PVPlacement(rotY,               // no rotation
		position_FieldWire_bottom4, // at (x,y,z)
		logic_FieldWire,       // its logical volume
		"phys__FieldWire_bottom4",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	G4VPhysicalVolume* phys_FieldWire_top1 = new G4PVPlacement(rotX,               // no rotation
		position_FieldWire_top1, // at (x,y,z)
		logic_FieldWire,       // its logical volume
		"phys__FieldWire_top1",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	G4VPhysicalVolume* phys_FieldWire_top2 = new G4PVPlacement(rotX,               // no rotation
		position_FieldWire_top2, // at (x,y,z)
		logic_FieldWire,       // its logical volume
		"phys__FieldWire_top2",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	G4VPhysicalVolume* phys_FieldWire_top3 = new G4PVPlacement(rotY,               // no rotation
		position_FieldWire_top3, // at (x,y,z)
		logic_FieldWire,       // its logical volume
		"phys__FieldWire_top3",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	G4VPhysicalVolume* phys_FieldWire_top4 = new G4PVPlacement(rotY,               // no rotation
		position_FieldWire_top4, // at (x,y,z)
		logic_FieldWire,       // its logical volume
		"phys__FieldWire_top4",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps


#endif //bFieldWires
	//--------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------
	//create FieldTHGEM
#ifdef bFieldTHGEM
	G4Box* solid_FieldTHGEM_out = new G4Box("solid_FieldTHGEM_out", x_size_FieldTHGEM / 2.0, y_size_FieldTHGEM / 2.0, z_size_FieldTHGEM / 2.0);
	G4Box* solid_FieldTHGEM_in = new G4Box("solid_FieldTHGEM_in", hole_size_FieldTHGEM / 2.0, hole_size_FieldTHGEM / 2.0, z_size_FieldTHGEM / 2.0);
	G4SubtractionSolid* solid_FieldTHGEM = new G4SubtractionSolid("solid_FieldTHGEM", solid_FieldTHGEM_out, solid_FieldTHGEM_in);

	G4LogicalVolume* logic_FieldTHGEM = new G4LogicalVolume(solid_FieldTHGEM, G4Material::GetMaterial("FR4"), "logic_FieldTHGEM", 0, 0, 0);

	G4VPhysicalVolume* phys_FieldTHGEM_1 = new G4PVPlacement(0,               // no rotation
		position_FieldTHGEM_1, // at (x,y,z)
		logic_FieldTHGEM,       // its logical volume
		"phys_FieldTHGEM_1",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 


	G4VPhysicalVolume* phys_FieldTHGEM_2 = new G4PVPlacement(0,               // no rotation
		position_FieldTHGEM_2, // at (x,y,z)
		logic_FieldTHGEM,       // its logical volume
		"phys_FieldTHGEM_2",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 
#endif
	 //--------------------------------------------------------------------------------


	 //-------------------------------------------------------------------------------
#ifdef	bSingleTHGEMHole
	G4Tubs* solid_SingleTHGEMHole_out
		= new G4Tubs("solid_SingleTHGEMHole_out", g()->radius_THGEM_hole, g()->radius_THGEM_hole*2, g()->width_THGEM1 / 2.0, 0. *deg, 360.*deg);
	G4LogicalVolume* logic_SingleTHGEMHole_out
		= new G4LogicalVolume(solid_SingleTHGEMHole_out, G4Material::GetMaterial("FR4"), "logic_SingleTHGEMHole_out", 0, 0, 0);
	G4VPhysicalVolume* phys_SingleTHGEMHole_out = new G4PVPlacement(0,               // no rotation
		position_SingleTHGEMHole, // at (x,y,z)
		logic_SingleTHGEMHole_out,       // its logical volume
		"phys_SingleTHGEMHole_out",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	G4Tubs* solid_SingleTHGEMHole_in
		= new G4Tubs("solid_SingleTHGEMHole_in", 0, g()->radius_THGEM_hole, g()->width_THGEM1 / 2.0, 0. *deg, 360.*deg);
	G4LogicalVolume* logic_SingleTHGEMHole_in
		= new G4LogicalVolume(solid_SingleTHGEMHole_in, G4Material::GetMaterial("Air"), "logic_SingleTHGEMHole_in", 0, 0, 0);
	G4VPhysicalVolume* phys_SingleTHGEMHole_in = new G4PVPlacement(0,               // no rotation
		position_SingleTHGEMHole, // at (x,y,z)
		logic_SingleTHGEMHole_in,       // its logical volume
		"phys_SingleTHGEMHole_in",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps


#endif
	 //--------------------------------------------------------------------------------



#ifdef	bAlpha
	
	G4Tubs* solidbAlpha
		= new G4Tubs("solidbAlpha", 0, radiusAlphaFull, z_size_Alpha / 2.0, 0. *deg, 360.*deg);

	G4LogicalVolume* logicbAlpha
		= new G4LogicalVolume(solidbAlpha, G4Material::GetMaterial("Air"), "logicbAlpha", 0, 0, 0);

	G4VPhysicalVolume* physbAlpha = new G4PVPlacement(0,               // no rotation
		positionbAlpha, // at (x,y,z)
		logicbAlpha,       // its logical volume
		"physbAlpha",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

#endif


	//-------------------------------------------------------------------------------
	//create bCathode
#ifdef bCathode
	G4Box* solid_bCathode
		= new G4Box("solid_bCathode", x_size_Cathode / 2.0, y_size_Cathode / 2.0, z_size_Cathode / 2.0);
	G4LogicalVolume* logic_bCathode
		= new G4LogicalVolume(solid_bCathode, G4Material::GetMaterial("FR4"), "logic_bCathode", 0, 0, 0);
	G4VPhysicalVolume* phys_bCathode = new G4PVPlacement(0,               // no rotation
		position_Cathode, // at (x,y,z)
		logic_bCathode,       // its logical volume
		"phys_bCathode",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
	 //-------------------------------------------------------------------------------
#endif // bCathode


#ifdef	bLArInactive
	G4Box* solid_LArInactive
		= new G4Box("solid_LArInactive", x_size_LArInactive / 2.0, y_size_LArInactive / 2.0, z_size_LArInactive / 2.0);
	G4LogicalVolume* logic_LArInactive
		= new G4LogicalVolume(solid_LArInactive, G4Material::GetMaterial(/*"LAr"*/"Air"), "logic_LArInactive", 0, 0, 0);
	G4VPhysicalVolume* phys_LArInactive = new G4PVPlacement(0,               // no rotation
		position_LArInactive, // at (x,y,z)
		logic_LArInactive,       // its logical volume
		"phys_LArInactive",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif // bLArInactive


#ifdef	bPMMA_bottom
	G4Box* solid_PMMA_bottom
		= new G4Box("solid_PMMA_bottom", x_size_PMMA_bottom / 2.0, y_size_PMMA_bottom / 2.0, z_size_PMMA_bottom / 2.0);
	G4LogicalVolume* logic_PMMA_bottom
		= new G4LogicalVolume(solid_PMMA_bottom, G4Material::GetMaterial("PMMA"), "logic_PMMA_bottom", 0, 0, 0);
	G4VPhysicalVolume* phys_PMMA_bottom = new G4PVPlacement(0,               // no rotation
		position_PMMA_bottom, // at (x,y,z)
		logic_PMMA_bottom,       // its logical volume
		"phys_PMMA_bottom",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif // bPMMA_bottom


#ifdef	bAl_window
	G4Tubs* solid_Al_window
		= new G4Tubs("solid_Al_window", 0, 	diameter_size_Al_window/2.0, z_size_Al_window/2.0, 0. *deg, 360.*deg);	
	G4LogicalVolume* logic_Al_window
		= new G4LogicalVolume(solid_Al_window, G4Material::GetMaterial("Al"), "logic_Al_window", 0, 0, 0);
	G4VPhysicalVolume* phys_Al_window_top = new G4PVPlacement(0,               // no rotation
		position_Al_window_top, // at (x,y,z)
		logic_Al_window,       // its logical volume
		"phys_Al_window_top",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
	G4VPhysicalVolume* phys_Al_window_bottom = new G4PVPlacement(0,               // no rotation
		position_Al_window_bottom, // at (x,y,z)
		logic_Al_window,       // its logical volume
		"phys_Al_window_bottom",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif // bAl_window

#ifdef bCryogenicChamberBottom
	G4Tubs* solid_CryogenicChamberBottom
		= new G4Tubs("solid_CryogenicChamberBottom", diameter_size_internal_CryogenicChamberBottom / 2.0, diameter_size_external_CryogenicChamberBottom / 2.0, z_size_CryogenicChamberBottom / 2.0, 0. *deg, 360.*deg);
	G4LogicalVolume* logic_CryogenicChamberBottom
		= new G4LogicalVolume(solid_CryogenicChamberBottom, G4Material::GetMaterial("Fe"), "logic_CryogenicChamberBottom", 0, 0, 0);
	G4VPhysicalVolume* phys_CryogenicChamberBottom = new G4PVPlacement(0,               // no rotation
		position_CryogenicChamberBottom, // at (x,y,z)
		logic_CryogenicChamberBottom,       // its logical volume
		"phys_CryogenicChamberBottom",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif // bCryogenicChamberBottom

#if defined(bExternalColl6mm) || defined(bExternalColl2mm)
	G4Tubs* solid_ExternalColl
		= new G4Tubs("solid_ExternalColl", diameter_inter_ExternalColl / 2.0, diameter_ExternalColl / 2.0, z_size_ExternalColl / 2.0, 0. *deg, 360.*deg);
	G4LogicalVolume* logic_ExternalColl
		= new G4LogicalVolume(solid_ExternalColl, G4Material::GetMaterial("Fe"), "logic_ExternalColl", 0, 0, 0);
	G4VPhysicalVolume* phys_ExternalColl = new G4PVPlacement(0,               // no rotation
		position_ExternalColl, // at (x,y,z)
		logic_ExternalColl,       // its logical volume
		"phys_ExternalColl",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif



#ifdef	bCd109
	G4Tubs* solid_Cd109
		= new G4Tubs("solid_Cd109", 0, diameter_size_Cd109 / 2.0, z_size_Cd109 / 2.0, 0. *deg, 360.*deg);
	G4LogicalVolume* logic_Cd109
		= new G4LogicalVolume(solid_Cd109, G4Material::GetMaterial("Air"), "logic_Cd109", 0, 0, 0);
	G4VPhysicalVolume* phys_Cd109 = new G4PVPlacement(0,               // no rotation
		position_Cd109, // at (x,y,z)
		logic_Cd109,       // its logical volume
		"phys_Cd109",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif //bCd109


#ifdef	bCd109BeFoil
	G4Tubs* solid_Cd109BeFoil
		= new G4Tubs("solid_Cd109BeFoil", 0, diameter_size_Cd109BeFoil / 2.0, z_size_Cd109BeFoil / 2.0, 0. *deg, 360.*deg);
	G4LogicalVolume* logic_Cd109BeFoil
		= new G4LogicalVolume(solid_Cd109BeFoil, G4Material::GetMaterial("Be"), "logic_Cd109BeFoil", 0, 0, 0);
	G4VPhysicalVolume* phys_Cd109BeFoil = new G4PVPlacement(0,               // no rotation
		position_Cd109BeFoil, // at (x,y,z)
		logic_Cd109BeFoil,       // its logical volume
		"phys_Cd109BeFoil",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif //bCd109BeFoil

#ifdef	bCd109WolframPlug
	G4Tubs* solid_Cd109WolframPlug
		= new G4Tubs("solid_Cd109WolframPlug", 0, diameter_size_Cd109WolframPlug / 2.0, z_size_Cd109WolframPlug / 2.0, 0. *deg, 360.*deg);
	G4LogicalVolume* logic_Cd109WolframPlug
		= new G4LogicalVolume(solid_Cd109WolframPlug, G4Material::GetMaterial("W"), "logic_Cd109WolframPlug", 0, 0, 0);
	G4VPhysicalVolume* phys_Cd109WolframPlug = new G4PVPlacement(0,               // no rotation
		position_Cd109WolframPlug, // at (x,y,z)
		logic_Cd109WolframPlug,       // its logical volume
		"phys_Cd109WolframPlug",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif //bCd109WolframPlug


#ifdef	bCd109IsotopBoxHolder
	G4Tubs* solid_Cd109IsotopBoxHolder
		= new G4Tubs("solid_Cd109IsotopBoxHolder", diameter_size_Cd109IsotopBoxHolder_top_hole / 2.0, diameter_size_Cd109IsotopBoxHolder/2.0, z_size_Cd109IsotopBoxHolder / 2.0, 0. *deg, 360.*deg);
	G4LogicalVolume* logic_Cd109IsotopBoxHolder
		= new G4LogicalVolume(solid_Cd109IsotopBoxHolder, G4Material::GetMaterial("Brass"), "logic_Cd109IsotopBoxHolder", 0, 0, 0);
	G4VPhysicalVolume* phys_Cd109IsotopBoxHolder = new G4PVPlacement(0,               // no rotation
		position_Cd109IsotopBoxHolder, // at (x,y,z)
		logic_Cd109IsotopBoxHolder,       // its logical volume
		"phys_Cd109IsotopBoxHolder",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	G4Tubs* solid_Cd109IsotopBoxHolder_plug
		= new G4Tubs("solid_Cd109IsotopBoxHolder_plug", 0, diameter_size_Cd109IsotopBoxHolder_top_hole / 2.0, (z_size_Cd109IsotopBoxHolder_bottom_hole - z_size_Cd109) / 2.0, 0. *deg, 360.*deg);
	G4LogicalVolume* logic_Cd109IsotopBoxHolder_plug
		= new G4LogicalVolume(solid_Cd109IsotopBoxHolder_plug, G4Material::GetMaterial("Brass"), "logic_Cd109IsotopBoxHolder_plug", 0, 0, 0);
	G4VPhysicalVolume* phys_Cd109IsotopBoxHolder_plug = new G4PVPlacement(0,               // no rotation
		position_Cd109IsotopBoxHolder_plug, // at (x,y,z)
		logic_Cd109IsotopBoxHolder_plug,       // its logical volume
		"phys_Cd109IsotopBoxHolder_plug",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

#endif //bCd109IsotopBoxHolder

	
#ifdef	bCd109CuFoil
	G4Box* solid_Cd109CuFoil
		= new G4Box("solid_Cd109CuFoil", x_size_Cd109CuFoil / 2.0, y_size_Cd109CuFoil / 2.0, z_size_Cd109CuFoil / 2.0);
	G4LogicalVolume* logic_Cd109CuFoil
		= new G4LogicalVolume(solid_Cd109CuFoil, G4Material::GetMaterial("Cu"), "logic_Cd109CuFoil", 0, 0, 0);
	G4VPhysicalVolume* phys_Cd109CuFoil = new G4PVPlacement(0,               // no rotation
		position_Cd109CuFoil, // at (x,y,z)
		logic_Cd109CuFoil,       // its logical volume
		"phys_Cd109CuFoil",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif //bCd109CuFoil


#ifdef	bCd109Detector
	G4Box* solid_Cd109Detector
		= new G4Box("solid_Cd109Detector", x_size_Cd109Detector / 2.0, y_size_Cd109Detector / 2.0, z_size_Cd109Detector / 2.0);
	G4LogicalVolume* logic_Cd109Detector
		= new G4LogicalVolume(solid_Cd109Detector, G4Material::GetMaterial("YAP_Ce_no_scint"), "logic_Cd109Detector", 0, 0, 0);
	G4VPhysicalVolume* phys_Cd109Detector = new G4PVPlacement(0,               // no rotation
		position_Cd109Detector, // at (x,y,z)
		logic_Cd109Detector,       // its logical volume
		"phys_Cd109Detector",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif //bCd109Detector


#ifdef bCd109InternalColl
	G4Tubs* solid_Cd109InternalColl
		= new G4Tubs("solid_Cd109InternalColl", diameter_size_internal_Cd109InternalColl / 2.0, diameter_size_external_Cd109InternalColl / 2.0, z_size_Cd109InternalColl / 2.0, 0. *deg, 360.*deg);
	G4LogicalVolume* logic_Cd109InternalColl
		= new G4LogicalVolume(solid_Cd109InternalColl, G4Material::GetMaterial("Fe"), "logic_Cd109InternalColl", 0, 0, 0);
	G4VPhysicalVolume* phys_Cd109InternalColl = new G4PVPlacement(0,               // no rotation
		position_Cd109InternalColl, // at (x,y,z)
		logic_Cd109InternalColl,       // its logical volume
		"phys_Cd109InternalColl",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
#endif //bCd109InternalColl


#ifdef bAnode_grid
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


						 //--------------------------------------------------------------------------------position_PMTAnodeGridTracker_0
#endif // bAnode_grid



#ifdef bPMTAnodeGrid
	//--------------------------------------------------------------------------------
	// create tracker_anode_grid (this need for anode_grid parametrising)
	
	//Gas
	G4Box* solid_PMTAnodeGridTrackerGas = new G4Box("solid_PMTAnodeGridTrackerGas", PMTAnodeGridTrackerGasXSize / 2.0, PMTAnodeGridTrackerGasYSize / 2.0, PMTAnodeGridTrackerThickness / 2.0);
	G4LogicalVolume* logic_PMTAnodeGridTrackerGas = new G4LogicalVolume(solid_PMTAnodeGridTrackerGas, G4Material::GetMaterial("Air"), "logic_PMTAnodeGridTrackerGas", 0, 0, 0);
	G4LogicalVolume* logic_PMTAnodeGridTrackerGasInner = new G4LogicalVolume(solid_PMTAnodeGridTrackerGas, G4Material::GetMaterial("Air"), "logic_PMTAnodeGridTrackerGasInner", 0, 0, 0);
	
	G4VPhysicalVolume* phys_PMTAnodeGridTrackerGas_1 = new G4PVPlacement(rotY,               // no rotation
		position_PMTAnodeGridTrackerGas_1, // at (x,y,z)
		logic_PMTAnodeGridTrackerGas,       // its logical volume
		"phys_PMTAnodeGridTrackerGas_1",       // its name
		logicWorld /*logic_LAr_outer*/,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	G4VPhysicalVolume* phys_PMTAnodeGridTrackerGasInner_1 = new G4PVPlacement(rotY,               // no rotation
		position_PMTAnodeGridTrackerGasInner_1, // at (x,y,z)
		logic_PMTAnodeGridTrackerGasInner,       // its logical volume
		"phys_PMTAnodeGridTrackerGasInner_1",       // its name
		logicWorld /*logic_LAr_outer*/,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	//Liquid
	G4Box* solid_PMTAnodeGridTrackerLiquid = new G4Box("solid_PMTAnodeGridTrackerLiquid", PMTAnodeGridTrackerLiquidXSize / 2.0, PMTAnodeGridTrackerLiquidYSize / 2.0, PMTAnodeGridTrackerThickness / 2.0);
	G4LogicalVolume* logic_PMTAnodeGridTrackerLiquid = new G4LogicalVolume(solid_PMTAnodeGridTrackerLiquid, G4Material::GetMaterial("LAr"), "logic_PMTAnodeGridTrackerLiquid", 0, 0, 0);
	G4LogicalVolume* logic_PMTAnodeGridTrackerLiquidInner = new G4LogicalVolume(solid_PMTAnodeGridTrackerLiquid, G4Material::GetMaterial("LAr"), "logic_PMTAnodeGridTrackerLiquidInner", 0, 0, 0);
	G4VPhysicalVolume* phys_PMTAnodeGridTrackerLiquid_1 = new G4PVPlacement(rotY,               // no rotation
		position_PMTAnodeGridTrackerLiquid_1, // at (x,y,z)
		logic_PMTAnodeGridTrackerLiquid,       // its logical volume
		"phys_PMTAnodeGridTrackerLiquid_1",       // its name
		/*logicWorld*/ logic_LAr_outer,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps	

	G4VPhysicalVolume* phys_PMTAnodeGridTrackerLiquidInner_1 = new G4PVPlacement(rotY,               // no rotation
		position_PMTAnodeGridTrackerLiquidInner_1, // at (x,y,z)
		logic_PMTAnodeGridTrackerLiquidInner,       // its logical volume
		"phys_PMTAnodeGridTrackerLiquidInner_1",       // its name
		/*logicWorld*/ logic_LAr_outer,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
						 
	//--------------------------------------------------------------------------------







	//--------------------------------------------------------------------------------
	//create PMTGridWire
	G4Tubs* solid_PMTGridWire = new G4Tubs("solid_PMTGridWire", 0, PMTGridWireRadius, PMTAnodeGridTrackerGasYSize / 2.0, 0.*deg, 360.*deg);
	G4LogicalVolume* logic_PMTGridWire = new G4LogicalVolume(solid_PMTGridWire, fAl, "lwire", 0, 0, 0);
	G4VPVParameterisation* param_PMTGridWireGas = new PMTGridParametrisation(PMTAnodeGridNCellsGas, 0, 0, 0, PMTGridWirePitch, PMTGridWireRadius, false);
	G4VPhysicalVolume* phys_PMTGridWireGas_0 = new G4PVParameterised("phys_PMTGridWireGas_0",       // their name
		logic_PMTGridWire,   // their logical volume
		logic_PMTAnodeGridTrackerGas,       // Mother logical volume
		kXAxis,          // Are placed along this axis 
		PMTAnodeGridNCellsGas,    // Number of chambers
		param_PMTGridWireGas,    // The parametrisation
		fCheckOverlaps); // checking overlaps	

	G4Tubs* solid_PMTGridWireGasInner = new G4Tubs("solid_PMTGridWireGasInner", 0, PMTGridWireRadius, PMTAnodeGridTrackerGasXSize/2.0, 0.*deg, 360.*deg);
	G4LogicalVolume* logic_PMTGridWireGasInner = new G4LogicalVolume(solid_PMTGridWireGasInner, fAl, "lwire", 0, 0, 0);
	G4VPVParameterisation* param_PMTGridWireGasInner = new PMTGridParametrisation(PMTAnodeGridNCellsGasInner, 0, 0, 0, PMTGridWirePitch, PMTGridWireRadius, true);
	G4VPhysicalVolume* phys_PMTGridWireGasInner_0 = new G4PVParameterised("phys_PMTGridWireGasInner_0",       // their name
		logic_PMTGridWireGasInner,   // their logical volume
		logic_PMTAnodeGridTrackerGasInner,       // Mother logical volume
		kXAxis,          // Are placed along this axis 
		PMTAnodeGridNCellsGasInner,    // Number of chambers
		param_PMTGridWireGasInner,    // The parametrisation
		fCheckOverlaps); // checking overlaps
	
	G4VPVParameterisation* param_PMTGridWireLiquid = new PMTGridParametrisation(PMTAnodeGridNCellsLiquid, 0, 0, 0, PMTGridWirePitch, PMTGridWireRadius, false);
	G4VPhysicalVolume* phys_PMTGridWireLiquid_0 = new G4PVParameterised("phys_PMTGridWireLiquid_0",       // their name
		logic_PMTGridWire,   // their logical volume
		logic_PMTAnodeGridTrackerLiquid,       // Mother logical volume
		kXAxis,          // Are placed along this axis 
		PMTAnodeGridNCellsLiquid,    // Number of chambers
		param_PMTGridWireLiquid,    // The parametrisation
		fCheckOverlaps); // checking overlaps

	G4Tubs* solid_PMTGridWireLiquidInner = new G4Tubs("solid_PMTGridWireLiquidInner", 0, PMTGridWireRadius, PMTAnodeGridTrackerLiquidXSize/2.0, 0.*deg, 360.*deg);
	G4LogicalVolume* logic_PMTGridWireLiquidInner = new G4LogicalVolume(solid_PMTGridWireLiquidInner, fAl, "lwire", 0, 0, 0);
	G4VPVParameterisation* param_PMTGridWireLiquidInner = new PMTGridParametrisation(PMTAnodeGridNCellsLiquidInner, 0, 0, 0, PMTGridWirePitch, PMTGridWireRadius, true);
	G4VPhysicalVolume* phys_PMTGridWireLiquidInner_0 = new G4PVParameterised("phys_PMTGridWireLiquidInner_0",       // their name
		logic_PMTGridWireLiquidInner,   // their logical volume
		logic_PMTAnodeGridTrackerLiquidInner,       // Mother logical volume
		kXAxis,          // Are placed along this axis 
		PMTAnodeGridNCellsLiquidInner,    // Number of chambers
		param_PMTGridWireLiquidInner,    // The parametrisation
		fCheckOverlaps); // checking overlaps

	//--------------------------------------------------------------------------------
#endif // bPMTAnodeGrid


#ifdef	bSteelBox
	G4Box* solidSteelBox
		= new G4Box("solidSteelBox", xSizeSteelBox / 2.0, ySizeSteelBox / 2.0, zSizeSteelBox / 2.0);

	G4Tubs* solidPMTtmp = new G4Tubs("solidPMTtmp", 0, radius_PMT, z_size_PMT / 2.0, 0.*deg, 360.*deg);

	G4VSolid* solidBoxSubtractPMT
		= new G4SubtractionSolid("solidBoxSubtractPMT", solidSteelBox, solidPMTtmp,
			rotY, G4ThreeVector(0., 0., 0.));


	G4LogicalVolume* logicSteelBox
		= new G4LogicalVolume(solidBoxSubtractPMT, G4Material::GetMaterial("Air"), "logicSteelBox", 0, 0, 0);
	
	G4VPhysicalVolume* physSteelBox0 = new G4PVPlacement(0,               // no rotation
		positionSteelBox0, // at (x,y,z)
		logicSteelBox,       // its logical volume
		"physSteelBox0",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps
	
	G4VPhysicalVolume* physSteelBox1 = new G4PVPlacement(0,               // no rotation
		positionSteelBox1, // at (x,y,z)
		logicSteelBox,       // its logical volume
		"physSteelBox1",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	G4VPhysicalVolume* physSteelBox2 = new G4PVPlacement(rotZ,               // no rotation
		positionSteelBox2, // at (x,y,z)
		logicSteelBox,       // its logical volume
		"physSteelBox2",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	G4VPhysicalVolume* physSteelBox3 = new G4PVPlacement(rotZ,               // no rotation
		positionSteelBox3, // at (x,y,z)
		logicSteelBox,       // its logical volume
		"physSteelBox3",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps


#endif


#ifdef bSiPM
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
	// create SiPM matrix

	solid_SiPM = new G4Box("sscintillator", size_SiPM / 2.0, size_SiPM / 2.0, thickness_SiPM / 2.0);
	logic_SiPM = new G4LogicalVolume(solid_SiPM, G4Material::GetMaterial("Air"), "lSiPM", 0, 0, 0);

	G4VPVParameterisation* chamberParam =
		new DetectorParametrisation(Nx_SiPMs, Ny_SiPMs, 0, 0, 0, chamberSpacing, size_SiPM);

	// dummy value : kZAxis -- modified by parameterised volume

	G4VPhysicalVolume* phys_SiPM = new G4PVParameterised("phys_SiPM",       // their name
		logic_SiPM,   // their logical volume
		trackerLV,       // Mother logical volume
		kXAxis,          // Are placed along this axis 
		Nx_SiPMs * Ny_SiPMs,    // Number of chambers
		chamberParam,    // The parametrisation
		fCheckOverlaps); // checking overlaps 


//--------------------------------------------------------------------------------
#endif //bSiPM







#ifdef bPMMA_plate
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
#endif // bPMMA_plate



	//--------------------------------------------------------------------------------
	//create THGEM2, THGEM1, THGEM0 tracker(active)

	G4Box* solid_tracker_THGEM2
		= new G4Box("tracker", x_size_tracker_THGEM2 / 2.0, y_size_tracker_THGEM2 / 2.0, z_size_tracker_THGEM2 / 2.0);
	G4LogicalVolume* logic_tracker_THGEM2
		= new G4LogicalVolume(solid_tracker_THGEM2, G4Material::GetMaterial("Air"), "logic_tracker_THGEM2", 0, 0, 0);


	G4LogicalVolume* logic_tracker_THGEM0_LAr
		= new G4LogicalVolume(solid_tracker_THGEM2, G4Material::GetMaterial("LAr"), "logic_tracker_THGEM0_LAr", 0, 0, 0);

#ifdef bCuReflector
	G4Box* solid_tracker_THGEM_Cu_reflector
		= new G4Box("solid_tracker_THGEM_Cu_reflector", x_size_tracker_THGEM2 / 2.0, y_size_tracker_THGEM2 / 2.0, z_size_tracker_THGEM_Cu_reflector / 2.0);
	G4LogicalVolume* logic_tracker_THGEM_Cu_reflector
		= new G4LogicalVolume(solid_tracker_THGEM_Cu_reflector, G4Material::GetMaterial("FR4"), "logic_tracker_THGEM_Cu_reflector", 0, 0, 0);
	
	G4VPhysicalVolume* phys_tracker_THGEM0_Cu_reflector = new G4PVPlacement(0,               // no rotation
		position_tracker_THGEM0_Cu_reflector, // at (x,y,z)
		logic_tracker_THGEM_Cu_reflector,       // its logical volume
		"phys_tracker_THGEM0_Cu_reflector",       // its name
		logic_tracker_THGEM0_LAr,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

	G4VPhysicalVolume* phys_tracker_THGEM1_Cu_reflector = new G4PVPlacement(0,               // no rotation
		position_tracker_THGEM1_Cu_reflector, // at (x,y,z)
		logic_tracker_THGEM_Cu_reflector,       // its logical volume
		"phys_tracker_THGEM1_Cu_reflector",       // its name
		logic_tracker_THGEM2,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps

#endif //bCuReflector
	
#ifdef bTHGEM0
	G4VPhysicalVolume* phys_tracker_THGEM0 = new G4PVPlacement(0,               // no rotation
		position_tracker_THGEM0, // at (x,y,z)
		logic_tracker_THGEM0_LAr,       // its logical volume
		"phys_tracker_THGEM0",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 
#endif //bTHGEM0


#ifdef	bTHGEM1
	G4VPhysicalVolume* phys_tracker_THGEM1 = new G4PVPlacement(0,               // no rotation
		position_tracker_THGEM1, // at (x,y,z)
		logic_tracker_THGEM2,       // its logical volume
		"phys_tracker_THGEM1",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 
#endif //bTHGEM1


#ifdef bTHGEM2
	G4VPhysicalVolume* phys_tracker_THGEM2 = new G4PVPlacement(0,               // no rotation
		position_tracker_THGEM2, // at (x,y,z)
		logic_tracker_THGEM2,       // its logical volume
		"phys_tracker_THGEM2",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 
#endif // bTHGEM2



	//--------------------------------------------------------------------------------




	//--------------------------------------------------------------------------------
	//create THGEM (region without holes)
	G4Box* solid_THGEM_without_holes_base
		= new G4Box("solid_THGEM_without_holes_base", x_size_THGEM_without_holes / 2.0, y_size_THGEM_without_holes / 2.0, z_size_THGEM_without_holes / 2.0);

	G4SubtractionSolid* solid_THGEM_without_holes = new G4SubtractionSolid("solid_THGEM_without_holes", solid_THGEM_without_holes_base, solid_tracker_THGEM2);

	G4LogicalVolume* logic_THGEM_without_holes
		= new G4LogicalVolume(solid_THGEM_without_holes, G4Material::GetMaterial("FR4"), "logic_THGEM_without_holes", 0, 0, 0);

	
#ifdef bTHGEM0
	G4VPhysicalVolume* phys_THGEM0_without_holes = new G4PVPlacement(0,               // no rotation
		position_tracker_THGEM0, // at (x,y,z)
		logic_THGEM_without_holes,       // its logical volume
		"phys_THGEM0_without_holes",       // its name
		logic_LAr_inner,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 
#endif //bTHGEM0

#ifdef	bTHGEM1
	G4VPhysicalVolume* phys_THGEM1_without_holes = new G4PVPlacement(0,               // no rotation
		position_tracker_THGEM1, // at (x,y,z)
		logic_THGEM_without_holes,       // its logical volume
		"phys_THGEM1_without_holes",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 
#endif //bTHGEM1


#ifdef bTHGEM2
	G4VPhysicalVolume* phys_THGEM2_without_holes = new G4PVPlacement(0,               // no rotation
		position_tracker_THGEM2, // at (x,y,z)
		logic_THGEM_without_holes,       // its logical volume
		"phys_THGEM2_without_holes",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 
#endif // bTHGEM2

	//--------------------------------------------------------------------------------




	////--------------------------------------------------------------------------------
	////create THGEM2
	////too difficult!
	//G4Box* solid_THGEM2_cell_base_material = new G4Box("solid_THGEM2_cell_base_material", x_size_THGEM_cell / 2.0, y_size_THGEM_cell / 2.0, z_size_cell_THGEM / 2.0);
	//G4Tubs* solid_THGEM2_cell_base_hole = new G4Tubs("solid_THGEM2_cell_base_hole", 0, hole_diameter_THGEM / 2.0, z_size_cell_THGEM / 2.0, 0.*deg, 360.*deg);

	//G4SubtractionSolid* solid_THGEM2_cell_base = new G4SubtractionSolid("solid_THGEM2_cell_base-hole", solid_THGEM2_cell_base_material, solid_THGEM2_cell_base_hole);

	//G4LogicalVolume* logic_THGEM2_cell_base = new G4LogicalVolume(solid_THGEM2_cell_base, G4Material::GetMaterial("FR4"), "logic_THGEM2_cell_base", 0, 0, 0);


	//G4VPVParameterisation* param_THGEM2_cell_base =
	//	new THGEMParametrisation(Nx_cells_THGEM2, Ny_cells_THGEM2, 0, 0, 0, step_THGEM);


	//G4VPhysicalVolume* phys_THGEM2_cell_base = new G4PVParameterised("phys_THGEM2_cell_base",       // their name
	//	logic_THGEM2_cell_base,   // their logical volume
	//	logic_tracker_THGEM2,       // Mother logical volume
	//	kXAxis,          // Are placed along this axis 
	//	Nx_cells_THGEM2 * Ny_cells_THGEM2,    // Number of chambers
	//	param_THGEM2_cell_base,    // The parametrisation
	//	fCheckOverlaps); // checking overlaps 


	////--------------------------------------------------------------------------------


#ifdef bInsulator_box
	//--------------------------------------------------------------------------------
	//create Insulator box
	G4Box* solid_Insulator_box_inner = new G4Box("solid_Insulator_box_inner", x_size_Insulator_box_inner / 2.0, y_size_Insulator_box_inner / 2.0, z_size_Insulator_box / 2.0);
	G4Box* solid_Insulator_box_outer = new G4Box("solid_Insulator_box_outer", x_size_Insulator_box_outer / 2.0, y_size_Insulator_box_outer / 2.0, z_size_Insulator_box / 2.0);
	G4SubtractionSolid* solid_Insulator_box_subtraction = new G4SubtractionSolid("solid_Insulator_box_subtraction", solid_Insulator_box_outer, solid_Insulator_box_inner);

	G4LogicalVolume* logic_Insulator_box = new G4LogicalVolume(solid_Insulator_box_subtraction, G4Material::GetMaterial("PMMA"), "logic_Insulator_box", 0, 0, 0);
	G4VPhysicalVolume* phys_Insulator_box = new G4PVPlacement(0,               // no rotation
		position_Insulator_box, // at (x,y,z)
		logic_Insulator_box,       // its logical volume
		"phys_Insulator_box",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	//--------------------------------------------------------------------------------
#endif // bInsulator_box





#ifdef bPMTs
	////--------------------------------------------------------------------------------
	//create PMTs
	G4Tubs* solid_PMT = new G4Tubs("solid_PMT", 0, radius_PMT, z_size_PMT / 2.0, 0.*deg, 360.*deg);
	G4LogicalVolume* logic_PMT0 = new G4LogicalVolume(solid_PMT, fAl, "logic_PMT0", 0, 0, 0);
	G4LogicalVolume* logic_PMT1 = new G4LogicalVolume(solid_PMT, fAl, "logic_PMT1", 0, 0, 0);
	G4LogicalVolume* logic_PMT2 = new G4LogicalVolume(solid_PMT, fAl, "logic_PMT2", 0, 0, 0);
	G4LogicalVolume* logic_PMT3 = new G4LogicalVolume(solid_PMT, fAl, "logic_PMT3", 0, 0, 0);

	G4VPhysicalVolume* phys_PMT0 = new G4PVPlacement(rotY,               // no rotation
		position_PMT_0, // at (x,y,z)
		logic_PMT0,       // its logical volume
		"phys_PMT0",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	G4VPhysicalVolume* phys_PMT1 = new G4PVPlacement(rotY,               // no rotation
		position_PMT_1, // at (x,y,z)
		logic_PMT1,       // its logical volume
		"phys_PMT1",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	G4VPhysicalVolume* phys_PMT2 = new G4PVPlacement(rotX,               // no rotation
		position_PMT_2, // at (x,y,z)
		logic_PMT2,       // its logical volume
		"phys_PMT2",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	G4VPhysicalVolume* phys_PMT3 = new G4PVPlacement(rotX,               // no rotation
		position_PMT_3, // at (x,y,z)
		logic_PMT3,       // its logical volume
		"phys_PMT3",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	////--------------------------------------------------------------------------------
#endif //bPMTs





	//определение чувствительного объема
#ifdef bSiPM
	G4String sensitiveDetectorName = "/detector/sensitiveDetector";
	theCathodeSD = new CathodeSD(sensitiveDetectorName, phys_SiPM, Nx_SiPMs * Ny_SiPMs);
	G4SDManager::GetSDMpointer()->AddNewDetector(theCathodeSD);
	logic_SiPM->SetSensitiveDetector(theCathodeSD);
#endif //bSiPM


	////------------------------------------------------------------------------










	//---------------------------------------------------------------------------
	//установка поверхностей

#ifdef bLArInner
	G4LogicalBorderSurface* LAr_inner2physiWorld = new G4LogicalBorderSurface("LAr_inner2physiWorld", phys_LAr_inner, physiWorld, LAr_OpticalSurface);
	G4LogicalBorderSurface* physiWorld2LAr_inner = new G4LogicalBorderSurface("physiWorld2LAr_inner", physiWorld, phys_LAr_inner, LAr_OpticalSurface);
#endif

#ifdef bSiPM
	//SiPM
	G4LogicalBorderSurface* tracker2SiPM = new G4LogicalBorderSurface("tracker2SiPM", phys_tracker, phys_SiPM, SiPM_OpticalSurface /*silicaCathodeMaterial*/);
#endif //bSiPM


#ifdef bCathode
	//bCathode
	G4LogicalBorderSurface* LAr_inner2bCathode = new G4LogicalBorderSurface("LAr_inner2bCathode", phys_LAr_inner, phys_bCathode, Cu_Cathode /*AbsorberMaterial*/);
	G4LogicalBorderSurface* bCathodeLAr_inner2 = new G4LogicalBorderSurface("bCathodeLAr_inner2", phys_bCathode, phys_LAr_inner, Cu_Cathode /*AbsorberMaterial*/);
#endif // bCathode


#ifdef bLArOuter
	G4LogicalBorderSurface* LAr_outer2bCathode = new G4LogicalBorderSurface("LAr_outer2bCathode", phys_LAr_outer, phys_bCathode, Cu_Cathode/*AbsorberMaterial*/);
#endif // bLArOuter
	

#ifdef bInsulator_box
	G4LogicalBorderSurface* Insulator_box2bCathode = new G4LogicalBorderSurface("Insulator_box2bCathode", phys_Insulator_box, phys_bCathode, Cu_Cathode/*AbsorberMaterial*/);
#endif //bInsulator_box

#ifdef bAlpha
	G4LogicalBorderSurface* LAr_inner2bAlpha = new G4LogicalBorderSurface("LAr_inner2bAlpha", phys_LAr_inner, physbAlpha, stainlessSteel);
#endif
	

#ifdef bCuReflector
	G4LogicalSkinSurface* CuReflector_THGEM0_surface = new G4LogicalSkinSurface("CuReflector_THGEM0_surface", logic_tracker_THGEM_Cu_reflector, Cu_THGEM);
#endif


#ifdef bPMTs
	//________________________________________________
	//PMT	
	
	G4LogicalBorderSurface* PMTAnodeGridTrackerGas_PMT1 = new G4LogicalBorderSurface("PMTAnodeGridTrackerGas_PMT1", phys_PMTAnodeGridTrackerGas_1, phys_PMT1, PMT_cathode);
	G4LogicalBorderSurface* PMTAnodeGridTrackerLiquid_PMT1 = new G4LogicalBorderSurface("PMTAnodeGridTrackerLiquid_PMT1", phys_PMTAnodeGridTrackerLiquid_1, phys_PMT1, PMT_cathode);
	
	G4LogicalBorderSurface* world2PMT0 = new G4LogicalBorderSurface("world2PMT0", physiWorld, phys_PMT0, PMT_cathode);
	//G4LogicalBorderSurface* world2PMT1 = new G4LogicalBorderSurface("world2PMT1", physiWorld, phys_PMT1, PMT_cathode);
	G4LogicalBorderSurface* world2PMT2 = new G4LogicalBorderSurface("world2PMT2", physiWorld, phys_PMT2, PMT_cathode);
	G4LogicalBorderSurface* world2PMT3 = new G4LogicalBorderSurface("world2PMT3", physiWorld, phys_PMT3, PMT_cathode);

#ifdef bLArOuter
	G4LogicalBorderSurface* LAr_outer2PMT0 = new G4LogicalBorderSurface("LAr_outer2PMT0", phys_LAr_outer, phys_PMT0, PMT_cathode);
	G4LogicalBorderSurface* LAr_outer2PMT1 = new G4LogicalBorderSurface("LAr_outer2PMT1", phys_LAr_outer, phys_PMT1, PMT_cathode);
	G4LogicalBorderSurface* LAr_outer2PMT2 = new G4LogicalBorderSurface("LAr_outer2PMT2", phys_LAr_outer, phys_PMT2, PMT_cathode);
	G4LogicalBorderSurface* LAr_outer2PMT3 = new G4LogicalBorderSurface("LAr_outer2PMT3", phys_LAr_outer, phys_PMT3, PMT_cathode);
#endif // bLArOuter

	//________________________________________________
#endif //bPMTs


#ifdef	bSteelBox
	G4LogicalBorderSurface* LAr_outer2SteelBox0 = new G4LogicalBorderSurface("LAr_outer2SteelBox0", phys_LAr_outer, physSteelBox0, stainlessSteel);
	G4LogicalBorderSurface* world2SteelBox0 = new G4LogicalBorderSurface("world2SteelBox0", physiWorld, physSteelBox0, stainlessSteel);
	
	G4LogicalBorderSurface* LAr_outer2SteelBox1 = new G4LogicalBorderSurface("LAr_outer2SteelBox1", phys_LAr_outer, physSteelBox1, stainlessSteel);
	G4LogicalBorderSurface* world2SteelBox1 = new G4LogicalBorderSurface("world2SteelBox1", physiWorld, physSteelBox1, stainlessSteel);

	G4LogicalBorderSurface* LAr_outer2SteelBox2 = new G4LogicalBorderSurface("LAr_outer2SteelBox2", phys_LAr_outer, physSteelBox2, stainlessSteel);
	G4LogicalBorderSurface* world2SteelBox2 = new G4LogicalBorderSurface("world2SteelBox2", physiWorld, physSteelBox2, stainlessSteel);

	G4LogicalBorderSurface* LAr_outer2SteelBox3 = new G4LogicalBorderSurface("LAr_outer2SteelBox3", phys_LAr_outer, physSteelBox3, stainlessSteel);
	G4LogicalBorderSurface* world2SteelBox3 = new G4LogicalBorderSurface("world2SteelBox3", physiWorld, physSteelBox3, stainlessSteel);


#endif


#ifdef	bAnode_grid
	//anode_grid
	G4LogicalBorderSurface* physiWorld2anode_grid = new G4LogicalBorderSurface("physiWorld2anode_grid", physiWorld, phys_anode_grid, /*AbsorberMaterial*/ FR4_unified);
	G4LogicalBorderSurface* tracker_anode_grid2wire = new G4LogicalBorderSurface("tracker_anode_grid2wire", phys_tracker_anode_grid, phys_wire, /*AbsorberMaterial*/ Anode_wire_unified);
	G4LogicalBorderSurface* tracker_anode_grid2anode_grid = new G4LogicalBorderSurface("tracker_anode_grid2anode_grid", phys_tracker_anode_grid, phys_anode_grid, AbsorberMaterial);
#endif //bAnode_grid


#ifdef bPMMA_plate
	G4LogicalBorderSurface* PMMA_plate2anode_grid = new G4LogicalBorderSurface("PMMA_plate2anode_grid", phys_PMMA_plate, phys_anode_grid, AbsorberMaterial);
#endif // bPMMA_plate

#ifdef	bSingleTHGEMHole
	G4LogicalBorderSurface* World_SingleTHGEMHole_out_surface = new G4LogicalBorderSurface("World_SingleTHGEMHole_out_surface", physiWorld, phys_SingleTHGEMHole_out, /*SingleTHGEMHole_optical_surface*/AbsorberMaterial);
	G4LogicalBorderSurface* SingleTHGEMHole_in_SingleTHGEMHole_out_surface = new G4LogicalBorderSurface("SingleTHGEMHole_in_SingleTHGEMHole_out_surface", phys_SingleTHGEMHole_in, phys_SingleTHGEMHole_out, /*SingleTHGEMHole_optical_surface*/FR4_unified);
	
#endif

	//THGEMs
#ifdef bTHGEM2
	G4LogicalBorderSurface* World2THGEM2_without_hole = new G4LogicalBorderSurface("World2THGEM2_without_hole", physiWorld, phys_THGEM2_without_holes, AbsorberMaterial);
#endif //bTHGEM2

#ifdef bTHGEM1
	G4LogicalBorderSurface* World2THGEM1_without_hole = new G4LogicalBorderSurface("World2THGEM1_without_hole", physiWorld, phys_THGEM1_without_holes, AbsorberMaterial);
#endif //bTHGEM1

#ifdef bTHGEM0
	G4LogicalBorderSurface* phys_LAr_inner2THGEM0_without_hole = new G4LogicalBorderSurface("phys_LAr_inner2THGEM0_without_hole", phys_LAr_inner, phys_THGEM0_without_holes, AbsorberMaterial);
#endif //bTHGEM0

	//FieldTHGEMs
#ifdef bFieldTHGEM
	G4LogicalBorderSurface* phys_LAr_inner2phys_FieldTHGEM_1 = new G4LogicalBorderSurface("phys_LAr_inner2phys_FieldTHGEM_1", phys_LAr_inner, phys_FieldTHGEM_1, AbsorberMaterial);
	G4LogicalBorderSurface* phys_LAr_inner2phys_FieldTHGEM_2 = new G4LogicalBorderSurface("phys_LAr_inner2phys_FieldTHGEM_2", phys_LAr_inner, phys_FieldTHGEM_2, AbsorberMaterial);
#endif
	//---------------------------------------------------------------------------

	//FieldWires
#ifdef	bFieldWires
	G4LogicalBorderSurface* phys_LAr_inner2phys_FieldWire_bottom1 = new G4LogicalBorderSurface("phys_LAr_inner2phys_FieldWire_bottom1", phys_LAr_inner, phys_FieldWire_bottom1, AbsorberMaterial);
	G4LogicalBorderSurface* phys_LAr_inner2phys_FieldWire_bottom2 = new G4LogicalBorderSurface("phys_LAr_inner2phys_FieldWire_bottom2", phys_LAr_inner, phys_FieldWire_bottom2, AbsorberMaterial);
	G4LogicalBorderSurface* phys_LAr_inner2phys_FieldWire_bottom3 = new G4LogicalBorderSurface("phys_LAr_inner2phys_FieldWire_bottom3", phys_LAr_inner, phys_FieldWire_bottom3, AbsorberMaterial);
	G4LogicalBorderSurface* phys_LAr_inner2phys_FieldWire_bottom4 = new G4LogicalBorderSurface("phys_LAr_inner2phys_FieldWire_bottom4", phys_LAr_inner, phys_FieldWire_bottom4, AbsorberMaterial);

	G4LogicalBorderSurface* phys_LAr_inner2phys_FieldWire_top1 = new G4LogicalBorderSurface("phys_LAr_inner2phys_FieldWire_top1", phys_LAr_inner, phys_FieldWire_top1, AbsorberMaterial);
	G4LogicalBorderSurface* phys_LAr_inner2phys_FieldWire_top2 = new G4LogicalBorderSurface("phys_LAr_inner2phys_FieldWire_top2", phys_LAr_inner, phys_FieldWire_top2, AbsorberMaterial);
	G4LogicalBorderSurface* phys_LAr_inner2phys_FieldWire_top3 = new G4LogicalBorderSurface("phys_LAr_inner2phys_FieldWire_top3", phys_LAr_inner, phys_FieldWire_top3, AbsorberMaterial);
	G4LogicalBorderSurface* phys_LAr_inner2phys_FieldWire_top4 = new G4LogicalBorderSurface("phys_LAr_inner2phys_FieldWire_top4", phys_LAr_inner, phys_FieldWire_top4, AbsorberMaterial);
#endif




	//------------------------------------------------------------------------------
	// установка атрибутов визуализации

#ifdef bAlpha
	G4VisAttributes* VisAtt_Alpha = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.9));
	logicbAlpha->SetVisAttributes(VisAtt_Alpha);
#endif

#ifdef bAnode_grid
	//anode grid
	G4VisAttributes* WireVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.8));
	logic_wire->SetVisAttributes(WireVisAtt);
	G4VisAttributes* AnodeGridVisAtt = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0, 0.8));
	logic_anode_grid->SetVisAttributes(AnodeGridVisAtt);
#endif // bAnode_grid

#ifdef bCuReflector
	G4VisAttributes* CuReflector_VisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0, 0.8));
	logic_tracker_THGEM_Cu_reflector->SetVisAttributes(CuReflector_VisAtt);
#endif

#ifdef	bSingleTHGEMHole
	G4VisAttributes* VisAtt_SingleTHGEMHoleIn = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.8));
	logic_SingleTHGEMHole_in->SetVisAttributes(VisAtt_SingleTHGEMHoleIn);
#endif 

#ifdef	bPMTs
	//PMT
	G4VisAttributes* PMT_VisAtt = new G4VisAttributes(G4Colour(0.8, 0.5, 0.2, 0.8));
	logic_PMT0->SetVisAttributes(PMT_VisAtt);
	logic_PMT1->SetVisAttributes(PMT_VisAtt);
	logic_PMT2->SetVisAttributes(PMT_VisAtt);
	logic_PMT3->SetVisAttributes(PMT_VisAtt);
#endif //bPMTs

#ifdef bSiPM
	trackerLV->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.5, 0.5, 0.8)));
#endif //bSiPM


	////THGEM
	G4VisAttributes* tracker_THGEM2_VisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, /*0.8*/0.8));
	logic_tracker_THGEM2->SetVisAttributes(tracker_THGEM2_VisAtt);
	logic_tracker_THGEM0_LAr->SetVisAttributes(tracker_THGEM2_VisAtt);
	//logic_tracker_THGEM0_LAr->SetVisAttributes(G4VisAttributes::GetInvisible());
	
	G4VisAttributes* THGEM_without_holes_VisAtt = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0, 0.8));
	logic_THGEM_without_holes->SetVisAttributes(THGEM_without_holes_VisAtt);
	//logic_THGEM_without_holes->SetVisAttributes(G4VisAttributes::GetInvisible());

	//LAr
	G4VisAttributes* LAr_inside_VisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.3));
	logic_LAr_inner->SetVisAttributes(LAr_inside_VisAtt);
	//logic_LAr_inner->SetVisAttributes(G4VisAttributes::GetInvisible());

#ifdef bPMTAnodeGrid
	//logic_PMTAnodeGridTrackerGas->SetVisAttributes(G4Colour(0.0, 0.0, 0.0, 0.0));
	//logic_PMTAnodeGridTrackerGasInner->SetVisAttributes(G4Colour(0.0, 0.0, 0.0, 0.0));
	logic_PMTAnodeGridTrackerLiquid->SetVisAttributes(LAr_inside_VisAtt);
	logic_PMTAnodeGridTrackerLiquidInner->SetVisAttributes(LAr_inside_VisAtt);
#endif bPMTAnodeGrid

#ifdef bLArOuter
	logic_LAr_outer->SetVisAttributes(LAr_inside_VisAtt);
#endif // bLArOuter
	
#ifdef bLArInactive
	logic_LArInactive->SetVisAttributes(LAr_inside_VisAtt);
#endif // bLArInactive

#ifdef bCathode
	logic_bCathode->SetVisAttributes(tracker_THGEM2_VisAtt);
#endif // bCathode

#ifdef bAl_window
	logic_Al_window->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 0.8)));
#endif // bAl_window

#ifdef bCryogenicChamberBottom
	logic_CryogenicChamberBottom->SetVisAttributes(new G4VisAttributes(G4Colour(0.3, 0.5, 0.7, 0.8)));
#endif // bCryogenicChamberBottom

#ifdef bCd109
	logic_Cd109->SetVisAttributes(tracker_THGEM2_VisAtt);
#endif // bCd109

#ifdef bCd109BeFoil
	logic_Cd109BeFoil->SetVisAttributes(new G4VisAttributes(G4Colour(238 / 255.0, 197 / 255.0, 145 / 255.0, 0.8)));
#endif // Cd109BeFoil
	
#ifdef bCd109InternalColl
	logic_Cd109InternalColl->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 0.5, 0.8)));
#endif // bCd109

#ifdef bCd109WolframPlug
	logic_Cd109WolframPlug->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.6, 0.5, 0.8)));
#endif // bCd109
	
#ifdef bCd109CuFoil
	logic_Cd109CuFoil->SetVisAttributes(new G4VisAttributes(G4Colour(218/255.0, 138/255.0, 103/255.0, 0.8)));
#endif // bCd109

#ifdef bCd109Detector
	logic_Cd109Detector->SetVisAttributes(new G4VisAttributes(G4Colour(0 / 255.0, 0 / 255.0, 128 / 255.0, 0.8)));
#endif // bCd109Detector


	//FieldTHGEM
#ifdef bFieldTHGEM
	G4VisAttributes* FieldTHGEM_VisAtt = new G4VisAttributes(G4Colour(1.0, 0.7, 0.3, 0.8));
	logic_FieldTHGEM->SetVisAttributes(FieldTHGEM_VisAtt);
#endif

#ifdef	bFieldWires
	G4VisAttributes* FieldWires_VisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.8));
	logic_FieldWire->SetVisAttributes(FieldWires_VisAtt);
#endif


	//logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	logicWorld->SetVisAttributes(G4Colour(0.0, 0.0, 0.0, 0.0));
	//	logic_Insulator_box->SetVisAttributes(G4VisAttributes::Invisible);



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



DetectorConstruction::DetectorConstruction()
{
	messenger = new Messenger(this);
}

DetectorConstruction::~DetectorConstruction()
{
	delete messenger;
}


