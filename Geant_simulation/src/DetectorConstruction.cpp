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
#include "THGEMParametrisation.h"
#include "PMTSD.h"
#include "Singleton.h"

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

//z >= 0
#define bSiPM
#define bPMMA_plate
#define bAnode_grid
#define bInsulator_box
//#define bPMTs
//#define bWLS
//#define bTHGEM2
#define bTHGEM1
#define bTHGEM0
//#define bFieldTHGEM
#define	bLArOuter 
#define bLArInner
#define bCathode

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

	//--------------------------------------------------------------------------------
	//выставление размеров объектов
	const G4double HalfWorldLength = 17 * cm;

	//anode wire
	const double radius_wire = 100/2.0 * um;//you can understand this from photo
	const double length_wire = 60 * mm /* 108*mm future case*/;  //60*mm /*real case*/;
	const double step_wire = 1 * mm;
	const int N_wire = length_wire/step_wire - 1 /* 107 future case*/;

	//Anode_grid
	const double thickness_anode_grid = 0.5 * mm;
	const double size_anode_grid = 124 * mm;
	const double size_anode_grid_hole = length_wire /*future case*/;  //60 * mm /*real case*/;
	const double z_anode_grid_bottom = 78.2 * mm /*78.2 in case of one THGEM*/ /*82.7*mm in case of two THGEM*/;
	const double z_anode_grid_center = z_anode_grid_bottom + thickness_anode_grid / 2.0;

	//PMMA plate
	const double x_size_PMMA_plate = size_anode_grid;
	const double y_size_PMMA_plate = size_anode_grid;
	const double z_size_PMMA_plate = 1.5 * mm;
	const double z_PMMA_plate_center = z_anode_grid_center + thickness_anode_grid / 2.0 + z_size_PMMA_plate / 2.0;


	//SiPMs
	const int Nx_SiPMs = 11;
	const int Ny_SiPMs = 11;
	const double thickness_SiPM = 1 * nm;
	const double size_SiPM = 6 * mm;
	const double chamberSpacing = 10 * mm;
	const double z_SiPM_bottom = z_anode_grid_bottom + thickness_anode_grid + z_size_PMMA_plate + (0.1*mm /*small gap between PMMA and SiPM*/) /* 85.7*mm in case of two THGEM*/;
	const double z_SiPM_center = z_SiPM_bottom + thickness_SiPM / 2.0;

	cout << "z_SiPM_bottom = " << z_SiPM_bottom << endl;

	//tracker SiPM
	const double x_size_tracker = Nx_SiPMs * chamberSpacing + size_SiPM / 2.0;
	const double y_size_tracker = Ny_SiPMs * chamberSpacing + size_SiPM / 2.0;
	const double z_size_tracker = 0.1 * mm;

	//tracker Anode_grid
	const double x_size_tracker_anode_grid = length_wire;
	const double y_size_tracker_anode_grid = x_size_tracker_anode_grid;
	const double z_size_tracker_anode_grid = thickness_anode_grid;

	




	//tracker THGEM2 (active region with holes)
	const double x_size_tracker_THGEM2 = 100 * mm;
	const double y_size_tracker_THGEM2 = 100 * mm;
	const double z_size_tracker_THGEM2 = 500 * um;
	const double z_tracker_THGEM2_center = 77.2 * mm + z_size_tracker_THGEM2 / 2.0;

	//THGEM0
	const double z_THGEM0_bottom = 50.2 * mm;

	//THGEM_without_holes
	const double x_size_THGEM_without_holes = size_anode_grid;
	const double y_size_THGEM_without_holes = size_anode_grid;
	const double z_size_THGEM_without_holes = z_size_tracker_THGEM2;

	//THGEM sizes
	//const double hole_diameter_THGEM = 500 * um;
	//const double rim_diameter_THGEM = 700 * um;
	//const double step_THGEM = 0.9 * mm;
	//const double x_size_THGEM_cell = step_THGEM;
	//const double y_size_THGEM_cell = step_THGEM * sqrt(3) / 2;
	//const double z_size_total_THGEM = 500 * um;
	//const double z_size_foil_THGEM = 35 * um;
	//const double z_size_cell_THGEM = z_size_total_THGEM - z_size_foil_THGEM * 2;
	//const unsigned int Nx_cells_THGEM2 = 5;
	//const unsigned int Ny_cells_THGEM2 = Nx_cells_THGEM2;

	//Insulator_box
	const double x_size_Insulator_box_inner = 143 * mm;
	const double y_size_Insulator_box_inner = x_size_Insulator_box_inner;
	const double thickness_Insulator_box = 2 * mm;
	const double x_size_Insulator_box_outer = x_size_Insulator_box_inner + thickness_Insulator_box * 2;
	const double y_size_Insulator_box_outer = x_size_Insulator_box_outer;
	const double z_size_Insulator_box = 150 * mm;
	const double z_Insulator_box_center = z_size_Insulator_box / 2.0;

	//PMTs
	const double radius_PMT = 45 * mm / 2.0;
	const double z_size_PMT = 1 * um;
	const double x_pos_PMT = 152 * mm / 2.0 + z_size_PMT / 2;
	const double y_pos_PMT = x_pos_PMT;
	const double z_pos_PMT = 27.2 * mm + 63 * mm / 2.0;

	//WLS
	/*const double radius_WLS = 70 * mm / 2.0;
	const double z_size_WLS = 100 * um;
	const double x_pos_WLS = 152 * mm / 2.0 + z_size_PMT / 2;
	const double y_pos_WLS = x_pos_WLS;
	const double z_pos_WLS = z_pos_PMT;*/

	//LAr_inner
	const double x_size_LAr_inner = x_size_Insulator_box_inner;
	const double y_size_LAr_inner = y_size_Insulator_box_inner;
	const double z_size_LAr_inner = z_THGEM0_bottom + z_size_tracker_THGEM2 + 4 * mm;

	//LArOuter
	const double x_size_LAr_outer_in = x_size_Insulator_box_outer;
	const double y_size_LAr_outer_in = y_size_Insulator_box_outer;
	const double x_size_LAr_outer_out = 152 * mm;
	const double y_size_LAr_outer_out = x_size_LAr_outer_out;
	const double z_size_LAr_outer = z_size_LAr_inner;

	//FieldTHGEM
	const double x_size_FieldTHGEM = size_anode_grid;
	const double y_size_FieldTHGEM = size_anode_grid;
	const double z_size_FieldTHGEM = z_size_tracker_THGEM2;
	const double z_center_FieldTHGEM_1 = 18.2*mm + z_size_FieldTHGEM / 2;
	const double z_center_FieldTHGEM_2 = 34.2*mm + z_size_FieldTHGEM / 2;
	const double hole_size_FieldTHGEM = 88 * mm;

	//Cathode
	const double x_size_Cathode = x_size_LAr_outer_out;
	const double y_size_Cathode = y_size_LAr_outer_out;
	const double z_size_Cathode = 0.5 * mm;
	
	//LArInactive
	const double x_size_LArInactive = x_size_LAr_outer_out;
	const double y_size_LArInactive = y_size_LAr_outer_out;
	const double z_size_LArInactive = 2 * mm;

	//PMMA_bottom
	const double x_size_PMMA_bottom = x_size_LAr_outer_out;
	const double y_size_PMMA_bottom = y_size_LAr_outer_out;
	const double z_size_PMMA_bottom = 3 * mm;
	const double PMMA_bottom_center = -z_size_Cathode - z_size_LArInactive - z_size_PMMA_bottom / 2.0;

	//Al_window
	const double diameter_size_Al_window = 50 * mm;
	const double z_size_Al_window = 1.0 * mm;
	const double z_space_Al_window = 21 * mm;
	const double Al_window_top_center = PMMA_bottom_center - z_size_PMMA_bottom / 2.0 - z_size_Al_window / 2.0;
	const double Al_window_bottom_center = Al_window_top_center - z_size_Al_window/2.0 - z_space_Al_window - z_size_Al_window / 2.0;

	//CryogenicChamberBottom
	const double diameter_size_internal_CryogenicChamberBottom = diameter_size_Al_window;
	const double diameter_size_external_CryogenicChamberBottom = x_size_LAr_inner * sqrt(2);
	const double z_size_CryogenicChamberBottom = 10 * mm;
	const double CryogenicChamberBottom_center = PMMA_bottom_center - z_size_PMMA_bottom / 2.0 - z_size_CryogenicChamberBottom / 2.0;

	//ExternalColl
	const double diameter_ExternalColl = diameter_size_Al_window;
	const double z_size_ExternalColl = 12 * mm;
#ifdef bExternalColl6mm
	const double diameter_inter_ExternalColl = 6 * mm;
#endif // bExternalColl6mm
#ifdef bExternalColl2mm
	const double diameter_inter_ExternalColl = 2 * mm;
#endif // bExternalColl2mm
	const double ExternalColl_center = Al_window_bottom_center - z_size_Al_window / 2.0 - z_size_ExternalColl / 2.0;
	const double ExternalColl_bottom = ExternalColl_center - z_size_ExternalColl / 2.0;
	cout << "ExternalColl_bottom = " << ExternalColl_bottom << endl;

	//Cd109ExternalBox
	const double z_size_Cd109ExternalBox_top_hole = 13 * mm;
	const double z_size_Cd109ExternalBox_middle_hole = 54 * mm;
	const double Cd109ExternalBox_bottom = ExternalColl_bottom - z_size_Cd109ExternalBox_top_hole - z_size_Cd109ExternalBox_middle_hole;
	cout << "Cd109ExternalBox_bottom = " << Cd109ExternalBox_bottom << endl;

	//Cd109IsotopBoxHolder
	const double z_size_Cd109IsotopBoxHolder_top_hole = 2 * mm;
	const double z_size_Cd109IsotopBoxHolder_middle_hole = 3 * mm;
	const double z_size_Cd109IsotopBoxHolder_bottom_hole = 12 * mm;
	const double z_size_Cd109IsotopBoxHolder = z_size_Cd109IsotopBoxHolder_top_hole + z_size_Cd109IsotopBoxHolder_middle_hole + z_size_Cd109IsotopBoxHolder_bottom_hole;//summ = 17*mm
	const double diameter_size_Cd109IsotopBoxHolder_top_hole = 7 * mm;
	const double diameter_size_Cd109IsotopBoxHolder = 14.3 * mm;
	const double Cd109IsotopBoxHolder_top_hole_bottom = Cd109ExternalBox_bottom + z_size_Cd109IsotopBoxHolder_bottom_hole + z_size_Cd109IsotopBoxHolder_middle_hole;
	const double Cd109IsotopBoxHolder_top_hole_center = Cd109IsotopBoxHolder_top_hole_bottom + z_size_Cd109IsotopBoxHolder_top_hole/2.0;
	const double Cd109IsotopBoxHolder_center = Cd109ExternalBox_bottom + z_size_Cd109IsotopBoxHolder/2.0;
	

	//Cd109
	const double diameter_size_Cd109 = 1 * mm;
	const double z_size_Cd109 = 0.1 * mm;
	const double Cd109_center = Cd109IsotopBoxHolder_top_hole_bottom - z_size_Cd109/2.0;
	g()->z_source_gamma = Cd109_center;
	g()->radius_source_gamma = diameter_size_Cd109/2.0;
	cout << "Cd109_center" << Cd109_center << endl;

	const double Cd109IsotopBoxHolder_plug_center = Cd109ExternalBox_bottom + z_size_Cd109IsotopBoxHolder_bottom_hole / 2.0 - z_size_Cd109;

	//Cd109BeFoil
	const double diameter_size_Cd109BeFoil = 3 * mm;
	const double z_size_Cd109BeFoil = 0.1 * mm;
	const double Cd109BeFoil_center = Cd109_center + z_size_Cd109/2.0 + z_size_Cd109BeFoil/2.0;

	//Cd109WolframPlug
	const double diameter_size_Cd109WolframPlug = 3 * mm;
	const double z_size_Cd109WolframPlug = 3 * mm;
	const double Cd109WolframPlug_center = Cd109_center - z_size_Cd109 / 2.0 - z_size_Cd109WolframPlug / 2.0;
	
	//Cd109InternalColl
	const double diameter_size_internal_Cd109InternalColl = 2 * mm;
	const double diameter_size_external_Cd109InternalColl = 14.5 * mm;
	const double z_size_Cd109InternalColl = 10 * mm;
	const double Cd109InternalColl_center = Cd109ExternalBox_bottom + z_size_Cd109IsotopBoxHolder + z_size_Cd109InternalColl/2.0;

	//Cd109CuFoil
	const double x_size_Cd109CuFoil = 16 * mm;
	const double y_size_Cd109CuFoil = x_size_Cd109CuFoil;
	const double z_size_Cd109CuFoil = 0.165 * mm;
	const double Cd109CuFoil_center = Cd109ExternalBox_bottom + z_size_Cd109IsotopBoxHolder + z_size_Cd109CuFoil/2.0;

	//bCd109Detector
	const double x_size_Cd109Detector = 2 * mm;
	const double y_size_Cd109Detector = 10 * mm;
	const double z_size_Cd109Detector = 2 * mm;
	const double Cd109Detector_center = Cd109CuFoil_center + z_size_Cd109CuFoil/2.0 + z_size_Cd109Detector /2.0;

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

	const G4ThreeVector &position_tracker_THGEM2 = G4ThreeVector(0, 0, z_tracker_THGEM2_center);
	const G4ThreeVector &position_tracker_THGEM1 = G4ThreeVector(0, 0, 72.7 * mm + z_size_tracker_THGEM2 / 2.0);
	const G4ThreeVector &position_tracker_THGEM0 = G4ThreeVector(0, 0, z_THGEM0_bottom + z_size_tracker_THGEM2 / 2.0 - z_size_LAr_inner / 2.0);

	const G4ThreeVector &position_Insulator_box = G4ThreeVector(0, 0, z_Insulator_box_center);

	const G4ThreeVector &position_LAr_inner = G4ThreeVector(0, 0, z_size_LAr_inner / 2.0);
	const G4ThreeVector &position_LAr_outer = G4ThreeVector(0, 0, z_size_LAr_inner / 2.0);

	const G4ThreeVector &position_Cathode = G4ThreeVector(0, 0, -z_size_Cathode / 2.0);
	const G4ThreeVector &position_LArInactive = G4ThreeVector(0, 0, -z_size_Cathode - z_size_LArInactive / 2.0);
	const G4ThreeVector &position_PMMA_bottom = G4ThreeVector(0, 0, PMMA_bottom_center);	
	const G4ThreeVector &position_Al_window_top = G4ThreeVector(0, 0, Al_window_top_center);
	const G4ThreeVector &position_Al_window_bottom = G4ThreeVector(0, 0, Al_window_bottom_center);
	const G4ThreeVector &position_CryogenicChamberBottom = G4ThreeVector(0, 0, CryogenicChamberBottom_center);	
	const G4ThreeVector &position_ExternalColl = G4ThreeVector(0, 0, ExternalColl_center);

	//Cd109
	const G4ThreeVector &position_Cd109 = G4ThreeVector(0, 0, Cd109_center);
	const G4ThreeVector &position_Cd109IsotopBoxHolder_top_hole = G4ThreeVector(0, 0, Cd109IsotopBoxHolder_top_hole_center);
	const G4ThreeVector &position_Cd109IsotopBoxHolder = G4ThreeVector(0, 0, Cd109IsotopBoxHolder_center);	
	const G4ThreeVector &position_Cd109IsotopBoxHolder_plug = G4ThreeVector(0, 0, Cd109IsotopBoxHolder_plug_center);
	const G4ThreeVector &position_Cd109InternalColl = G4ThreeVector(0, 0, Cd109InternalColl_center);
	const G4ThreeVector &position_Cd109BeFoil = G4ThreeVector(0, 0, Cd109BeFoil_center);
	
	
	//Cd109Detector
	const G4ThreeVector &position_Cd109Detector = G4ThreeVector(0, 0, Cd109Detector_center);

	//Cd109CuFoil
	const G4ThreeVector &position_Cd109CuFoil = G4ThreeVector(0, 0, Cd109CuFoil_center);
	
	
	//Cd109WolframPlug
	const G4ThreeVector &position_Cd109WolframPlug = G4ThreeVector(0, 0, Cd109WolframPlug_center);

	//PMT
	const G4ThreeVector &position_PMT_0 = G4ThreeVector(-x_pos_PMT, 0, z_pos_PMT);
	const G4ThreeVector &position_PMT_1 = G4ThreeVector(x_pos_PMT, 0, z_pos_PMT);
	const G4ThreeVector &position_PMT_2 = G4ThreeVector(0, -y_pos_PMT, z_pos_PMT);
	const G4ThreeVector &position_PMT_3 = G4ThreeVector(0, y_pos_PMT, z_pos_PMT);


	const G4ThreeVector &position_FieldTHGEM_1 = G4ThreeVector(0, 0, z_center_FieldTHGEM_1 - z_size_LAr_inner / 2.0);
	const G4ThreeVector &position_FieldTHGEM_2 = G4ThreeVector(0, 0, z_center_FieldTHGEM_2 - z_size_LAr_inner / 2.0);

	G4RotationMatrix* rotX = new G4RotationMatrix();
	rotX->rotateX(90 * deg);

	G4RotationMatrix* rotY = new G4RotationMatrix();
	rotY->rotateY(90 * deg);
	//-------------------------------------------------------------------------------


	G4double a;  // atomic mass
	G4double z;  // atomic number
	G4double density;
	G4Material* fAl = new G4Material("Al", z = 13., a = 26.98*g / mole, density = 2.7*g / cm3);
	G4Material* fFe = new G4Material("Fe", z = 26., a = 55.85*g / mole, density = 7.874 *g / cm3);
	G4Material* fW = new G4Material("W", z = 74., a = 183.84*g / mole, density = 19.35 *g / cm3);
	G4Material* fCu = new G4Material("Cu", z = 29., a = 63.5*g / mole, density = 8.92 *g / cm3);
	G4Material* fBe = new G4Material("Be", z = 4., a = 9.01*g / mole, density = 1.85 *g / cm3);

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




#ifdef bLArOuter
	//--------------------------------------------------------------------------------
	//create LAr box contaned outside the PMMA insulator
	cout << x_size_LAr_outer_out / 2.0 << "\t" << y_size_LAr_outer_out / 2.0 << endl;
	G4Box* solid_LAr_outer_out = new G4Box("solid_LAr_outer_out", x_size_LAr_outer_out / 2.0, y_size_LAr_outer_out / 2.0, z_size_LAr_outer / 2.0);
	G4Box* solid_LAr_outer_in = new G4Box("solid_LAr_outer_in", x_size_LAr_outer_in / 2.0, y_size_LAr_outer_in / 2.0, z_size_LAr_outer / 2.0);
	G4SubtractionSolid* solid_LAr_outer = new G4SubtractionSolid("solid_LAr_outer", solid_LAr_outer_out, solid_LAr_outer_in);

	G4LogicalVolume* logic_LAr_outer = new G4LogicalVolume(solid_LAr_outer, G4Material::GetMaterial("LAr"), "logic_LAr_outer", 0, 0, 0);
	G4VPhysicalVolume* phys_LAr_outer = new G4PVPlacement(0,               // no rotation
		position_LAr_outer, // at (x,y,z)
		logic_LAr_outer,       // its logical volume
		"phys_LAr_outer",       // its name
		logicWorld,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

	//--------------------------------------------------------------------------------
#endif // bLArOuter



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


						 //--------------------------------------------------------------------------------
#endif // bAnode_grid




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

#ifdef bSiPM
	//SiPM
	G4LogicalBorderSurface* tracker2SiPM = new G4LogicalBorderSurface("tracker2SiPM", phys_tracker, phys_SiPM, SiPM_OpticalSurface /*silicaCathodeMaterial*/);
#endif //bSiPM


#ifdef bCathode
	//bCathode
	G4LogicalBorderSurface* LAr_inner2bCathode = new G4LogicalBorderSurface("LAr_inner2bCathode", phys_LAr_inner, phys_bCathode, AbsorberMaterial);
#endif // bCathode


#ifdef bLArOuter
	G4LogicalBorderSurface* LAr_outer2bCathode = new G4LogicalBorderSurface("LAr_outer2bCathode", phys_LAr_outer, phys_bCathode, AbsorberMaterial);
#endif // bLArOuter
	

#ifdef bInsulator_box
	G4LogicalBorderSurface* Insulator_box2bCathode = new G4LogicalBorderSurface("Insulator_box2bCathode", phys_Insulator_box, phys_bCathode, AbsorberMaterial);
#endif //bInsulator_box


#ifdef bPMTs
	//________________________________________________
	//PMT
	G4LogicalBorderSurface* world2PMT0 = new G4LogicalBorderSurface("world2PMT0", physiWorld, phys_PMT0, silicaCathodeMaterial);
	G4LogicalBorderSurface* world2PMT1 = new G4LogicalBorderSurface("world2PMT1", physiWorld, phys_PMT1, silicaCathodeMaterial);
	G4LogicalBorderSurface* world2PMT2 = new G4LogicalBorderSurface("world2PMT2", physiWorld, phys_PMT2, silicaCathodeMaterial);
	G4LogicalBorderSurface* world2PMT3 = new G4LogicalBorderSurface("world2PMT3", physiWorld, phys_PMT3, silicaCathodeMaterial);

#ifdef bLArOuter
	G4LogicalBorderSurface* LAr_outer2PMT0 = new G4LogicalBorderSurface("LAr_outer2PMT0", phys_LAr_outer, phys_PMT0, silicaCathodeMaterial);
	G4LogicalBorderSurface* LAr_outer2PMT1 = new G4LogicalBorderSurface("LAr_outer2PMT1", phys_LAr_outer, phys_PMT1, silicaCathodeMaterial);
	G4LogicalBorderSurface* LAr_outer2PMT2 = new G4LogicalBorderSurface("LAr_outer2PMT2", phys_LAr_outer, phys_PMT2, silicaCathodeMaterial);
	G4LogicalBorderSurface* LAr_outer2PMT3 = new G4LogicalBorderSurface("LAr_outer2PMT3", phys_LAr_outer, phys_PMT3, silicaCathodeMaterial);
#endif // bLArOuter

	//________________________________________________
#endif //bPMTs



#ifdef	bAnode_grid
	//anode_grid
	G4LogicalBorderSurface* physiWorld2anode_grid = new G4LogicalBorderSurface("physiWorld2anode_grid", physiWorld, phys_anode_grid, AbsorberMaterial);
	G4LogicalBorderSurface* tracker_anode_grid2wire = new G4LogicalBorderSurface("tracker_anode_grid2wire", phys_tracker_anode_grid, phys_wire, AbsorberMaterial);
	G4LogicalBorderSurface* tracker_anode_grid2anode_grid = new G4LogicalBorderSurface("tracker_anode_grid2anode_grid", phys_tracker_anode_grid, phys_anode_grid, AbsorberMaterial);
#endif //bAnode_grid


#ifdef bPMMA_plate
	G4LogicalBorderSurface* PMMA_plate2anode_grid = new G4LogicalBorderSurface("PMMA_plate2anode_grid", phys_PMMA_plate, phys_anode_grid, AbsorberMaterial);
#endif // bPMMA_plate



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




	//------------------------------------------------------------------------------
	// установка атрибутов визуализации

#ifdef bAnode_grid
	//anode grid
	G4VisAttributes* WireVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.8));
	logic_wire->SetVisAttributes(WireVisAtt);
	G4VisAttributes* AnodeGridVisAtt = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0, 0.8));
	logic_anode_grid->SetVisAttributes(AnodeGridVisAtt);
#endif // bAnode_grid


#ifdef	bPMTs
	//PMT
	G4VisAttributes* PMT_VisAtt = new G4VisAttributes(G4Colour(0.8, 0.5, 0.2, 0.8));
	logic_PMT0->SetVisAttributes(PMT_VisAtt);
	logic_PMT1->SetVisAttributes(PMT_VisAtt);
	logic_PMT2->SetVisAttributes(PMT_VisAtt);
	logic_PMT3->SetVisAttributes(PMT_VisAtt);
#endif //bPMTs



	////THGEM
	G4VisAttributes* tracker_THGEM2_VisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.8));
	logic_tracker_THGEM2->SetVisAttributes(tracker_THGEM2_VisAtt);
	logic_tracker_THGEM0_LAr->SetVisAttributes(tracker_THGEM2_VisAtt);
	G4VisAttributes* THGEM_without_holes_VisAtt = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0, 0.8));
	logic_THGEM_without_holes->SetVisAttributes(THGEM_without_holes_VisAtt);


	//LAr
	G4VisAttributes* LAr_inside_VisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.3));
	logic_LAr_inner->SetVisAttributes(LAr_inside_VisAtt);

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

