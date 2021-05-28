#ifndef DetectorConstruction_h
#define DetectorConstruction_h

#include <globals.hh>
#include <G4ThreeVector.hh>
#include <G4VUserDetectorConstruction.hh>
#include "CathodeSD.hh"
#include "DetectorParametrisation.h"


class G4Trap;
class G4Trd;
class G4Box;
class G4Tubs;
class G4Cons;
class G4Ellipsoid;
class G4SubtractionSolid;
class G4UnionSolid;
class G4BooleanSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4VPVParameterisation;
class G4UserLimits;
class Messenger;
class G4OpticalSurface;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:

	DetectorConstruction();
	~DetectorConstruction();

public:

	G4VPhysicalVolume* Construct();
	void ChangeDetectorConstruction(double parametr);
	void ChangeSurface(double parametr);
	void ChangeMaterials();

	void ChangeCathRefl();

private:

	void defineMaterials();
	void defineSurfaces();
	void SetSizeAndPosition();

	G4Material* fAl;
	G4Material* fFe;
	G4Material* fW;
	G4Material* fCu;
	G4Material* fBe;
	
	G4Box*             solidWorld;    // pointer to the solid envelope 
	G4LogicalVolume*   logicWorld;    // pointer to the logical envelope
	G4VPhysicalVolume* physiWorld;    // pointer to the physical envelope

	//G4Box* solidBoxScint;
	G4Cons*            solidConicalScint;   // pointer to the conic solid Scintillator
	G4LogicalVolume*   logicScint;   // pointer to the logical Scintillator
	G4VPhysicalVolume* physiScint;   // pointer to the physical Scintillator

	G4LogicalVolume*   logicBox;   // pointer to the logical Scintillator
	G4VPhysicalVolume* physiBox;   // pointer to the physical Scintillator

	G4Trd* solidTrdScint;
	G4Box*            solid_scintillator;   // pointer to the conic solid Scintillator


	G4Cons*             solidConicalShellShaft;  // pointer to the conic solid Shell
	G4Cons*             solidConicalShellCap;  // pointer to the solid Cap
	G4BooleanSolid*     solidShell; // pointer to the ellipsoidal solid Shell
	G4LogicalVolume*    logicShell;  // pointer to the logical Shell
	G4VPhysicalVolume*  physiShell;  // pointer to the physical Shell
	G4Cons*             solidAirCap;  // pointer to the solid Air between Scint and Cap
	G4LogicalVolume*    logicAirCap;  // pointer to the logical Air between Scint and Cap
	G4VPhysicalVolume*  physiAirCap;  // pointer to the physical Air between Scint and Cap

	G4Box*             Box_solidGrease;
	G4Tubs*            solid_grease;
	G4LogicalVolume*   logic_grease;
	G4VPhysicalVolume* physi_grease;

	G4Tubs*            solid_glass;
	G4Tubs*            solidShaft;
	G4UnionSolid*      solidEnvelope;
	G4LogicalVolume*   logic_glass;
	G4VPhysicalVolume* physi_glass;

	G4LogicalVolume*   logicWindow;
	G4VPhysicalVolume* physiWindow;


	G4Tubs*            solidCathode;  // pointer to the solid Cathode
	G4LogicalVolume*   logicCathode;  // pointer to the logical Cathode
	G4VPhysicalVolume* physiCathode;  // pointer to the physical Cathode

	G4Tubs*            solidCase;     // pointer to the solid Case
	G4LogicalVolume*   logicCase;     // pointer to the logical Case
	G4VPhysicalVolume* physiCase;     // pointer to the physical Case


	
	G4LogicalVolume*   logicAbs;     // pointer to the logical Case
	G4VPhysicalVolume* physiAbs;     // pointer to the physical Case


	Messenger* messenger;  // pointer to the Messenger

	G4String sScintillationMaterial;
	G4String sScintillatorFaceSurface;
	G4String sScintillatorLateralSurface;


	G4Box*            solid_SiPM;   // pointer to the box solid SiPM
	G4LogicalVolume*   logic_SiPM;   // pointer to the logical Scintillator
	G4VPhysicalVolume* physi_SiPM;   // pointer to the physical Scintillator

	G4Box*            solid_anode_grid;   
	G4LogicalVolume*   logic_anode_grid;
	G4VPhysicalVolume* phys_anode_grid;




	// surfaces
	G4OpticalSurface *world_scintillator;
	G4OpticalSurface *scintillator_grease;
	G4OpticalSurface *grease_glass;
	G4OpticalSurface *glass_cathode;


	// arbitraryly valued variables (not changeable by user)
	//G4double fWorldLength;            // Full length of the world volume
	//G4double fPMTLength;
	//G4double fCathodeThickness;

	//G4double fScint_x;
	//G4double fScint_y;

	CathodeSD* theCathodeSD;
	G4Region*  fRegion;

	G4OpticalSurface *BGOPolishedAirTeflon; // polished BGO surface wrapped with teflon
	G4OpticalSurface *BGOGroundAirTeflon;   // ground BGO surface wrapped with teflon
	G4OpticalSurface *polishedAir;          // polished crystal surface, not wrapped
	G4OpticalSurface *groundAir;            // ground crystal surface, not wrapped	 
	G4OpticalSurface *groundWhitePainted;   // ground crystal surface painted white
	G4OpticalSurface *polishedBlackPainted; // polished crystal surface painted black
	G4OpticalSurface *groundBlackPainted;   // ground crystal surface painted black
	G4OpticalSurface *Polished_Air_TiO2;
	G4OpticalSurface *Ground_Air_TiO2;
	G4OpticalSurface *airGroundAluminum; // ground aluminm surface
	//G4OpticalSurface *silicaCathodeMaterial; // surface between window and cathode
	G4OpticalSurface *PMT_cathode;
	G4OpticalSurface *SiPM_OpticalSurface;
	G4OpticalSurface *AbsorberMaterial;
	G4OpticalSurface *SingleTHGEMHole_optical_surface;
	G4OpticalSurface *teflon_unified;
	G4OpticalSurface *MgO_unified;
	G4OpticalSurface *TiO2_unified;
	G4OpticalSurface *FR4_unified;
	G4OpticalSurface *Anode_wire_unified;
	G4OpticalSurface *Cu_THGEM;
	G4OpticalSurface *LAr_OpticalSurface;
	G4OpticalSurface *Cu_Cathode;
	G4OpticalSurface *Glass_surface;
	G4OpticalSurface *stainlessSteel;

	G4MaterialPropertiesTable* luyag_pr;
	G4MaterialPropertiesTable *PMT_cathodeMaterialProperty;


	G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps




	//consts
	G4double HalfWorldLength;

	//anode wire
	double radius_wire;
	double length_wire;
	double step_wire;
	int N_wire; 

	//Anode_grid
	double thickness_anode_grid;
	double size_anode_grid;
	double size_anode_grid_hole;
	double z_anode_grid_bottom;

	//PMTGridWire
	double PMTGridWireRadius;
	double PMTGridWirePitch;

	//PMTAnodeGridTracker
	double PMTAnodeGridTrackerThickness;
	double PMTAnodeGridTrackerGasXSize;
	double PMTAnodeGridTrackerGasYSize;
	double PMTAnodeGridTrackerZbottom;	
	int PMTAnodeGridNCellsGas;
	int PMTAnodeGridNCellsGasInner;
	int PMTAnodeGridNCellsLiquid;
	int PMTAnodeGridNCellsLiquidInner;
	double PMTAnodeGridTrackerLiquidXSize;
	double PMTAnodeGridTrackerLiquidYSize;

	//PMMA plate
	double x_size_PMMA_plate;
	double y_size_PMMA_plate;
	double z_size_PMMA_plate;
	

	//SiPMs
	int Nx_SiPMs;
	int Ny_SiPMs;
	double thickness_SiPM;
	double size_SiPM;
	double chamberSpacing;	

	//tracker SiPM
	double x_size_tracker;
	double y_size_tracker;
	double z_size_tracker;

	//tracker Anode_grid
	double x_size_tracker_anode_grid;
	double y_size_tracker_anode_grid;
	double z_size_tracker_anode_grid;

	//tracker THGEM2 (active region with holes)
	double x_size_tracker_THGEM2;
	double y_size_tracker_THGEM2;
	double z_size_tracker_THGEM2;

	//SteelBox
	double xSizeSteelBox;
	double ySizeSteelBox;
	double zSizeSteelBox;
	

	//solid_tracker_THGEM_Cu_reflector
	double z_size_tracker_THGEM_Cu_reflector = z_size_tracker_THGEM2 / 10.0;

	//THGEM_without_holes
	double x_size_THGEM_without_holes;
	double y_size_THGEM_without_holes;
	double z_size_THGEM_without_holes;

	//Insulator_box
	double x_size_Insulator_box_inner;
	double y_size_Insulator_box_inner;
	double thickness_Insulator_box;
	double x_size_Insulator_box_outer;
	double y_size_Insulator_box_outer;
	double z_size_Insulator_box;
	double z_Insulator_box_center;

	//PMTs
	double radius_PMT;
	double z_size_PMT;
	double x_pos_PMT;
	double y_pos_PMT;
	double z_pos_PMT;

	//LAr_inner
	double x_size_LAr_inner;
	double y_size_LAr_inner;
	double z_size_LAr_inner;

	//LArOuter
	double x_size_LAr_outer_in;
	double y_size_LAr_outer_in;
	double x_size_LAr_outer_out;
	double y_size_LAr_outer_out;
	double z_size_LAr_outer;

	//FieldTHGEM
	double x_size_FieldTHGEM;
	double y_size_FieldTHGEM;
	double z_size_FieldTHGEM;
	double z_center_FieldTHGEM_1;
	double z_center_FieldTHGEM_2;
	double hole_size_FieldTHGEM;

	//FieldWire
	double radius_FieldWire;
	double length_FieldWire;
	double x_pos_FieldWire;
	double z_pos_FieldWire_bottom;
	double z_pos_FieldWire_top;

	//Cathode
	double x_size_Cathode;
	double y_size_Cathode;
	double z_size_Cathode;

	//LArInactive
	double x_size_LArInactive;
	double y_size_LArInactive; 
	double z_size_LArInactive;

	//PMMA_bottom
	double x_size_PMMA_bottom;
	double y_size_PMMA_bottom;
	double z_size_PMMA_bottom;
	double PMMA_bottom_center;

	//Al_window
	double diameter_size_Al_window;
	double z_size_Al_window;
	double z_space_Al_window;
	double Al_window_top_center;
	double Al_window_bottom_center;

	//CryogenicChamberBottom
	double diameter_size_internal_CryogenicChamberBottom;
	double diameter_size_external_CryogenicChamberBottom;
	double z_size_CryogenicChamberBottom;
	double CryogenicChamberBottom_center;

	//ExternalColl
	double diameter_ExternalColl;
	double z_size_ExternalColl;
	double ExternalColl_center;
	double ExternalColl_bottom;

	//Cd109ExternalBox
	double z_size_Cd109ExternalBox_top_hole;
	double z_size_Cd109ExternalBox_middle_hole;
	double Cd109ExternalBox_bottom;
	
	//Cd109IsotopBoxHolder
	double z_size_Cd109IsotopBoxHolder_top_hole;
	double z_size_Cd109IsotopBoxHolder_middle_hole;
	double z_size_Cd109IsotopBoxHolder_bottom_hole;
	double z_size_Cd109IsotopBoxHolder;
	double diameter_size_Cd109IsotopBoxHolder_top_hole;
	double diameter_size_Cd109IsotopBoxHolder;
	double Cd109IsotopBoxHolder_top_hole_bottom;
	double Cd109IsotopBoxHolder_top_hole_center;
	double Cd109IsotopBoxHolder_center;

	//Cd109
	double diameter_size_Cd109;
	double z_size_Cd109;
	double Cd109_center;
	double Cd109IsotopBoxHolder_plug_center;

	//Cd109BeFoil
	double diameter_size_Cd109BeFoil;
	double z_size_Cd109BeFoil;
	double Cd109BeFoil_center;

	//Cd109WolframPlug
	double diameter_size_Cd109WolframPlug;
	double z_size_Cd109WolframPlug;
	double Cd109WolframPlug_center;

	//Cd109InternalColl
	double diameter_size_internal_Cd109InternalColl;
	double diameter_size_external_Cd109InternalColl;
	double z_size_Cd109InternalColl;
	double Cd109InternalColl_center;

	//Cd109CuFoil
	double x_size_Cd109CuFoil;
	double y_size_Cd109CuFoil;
	double z_size_Cd109CuFoil;
	double Cd109CuFoil_center;

	//bCd109Detector
	double x_size_Cd109Detector;
	double y_size_Cd109Detector;
	double z_size_Cd109Detector;
	double Cd109Detector_center;

	//
	double radiusAlphaFull;
	double z_size_Alpha;


	G4ThreeVector position_SingleTHGEMHole;
	
	G4ThreeVector positionTracker;
	G4ThreeVector position_PMMA_plate;
	G4ThreeVector position_tracker_THGEM2;
	G4ThreeVector position_tracker_THGEM1;
	G4ThreeVector position_tracker_THGEM0;
	G4ThreeVector position_tracker_THGEM0_Cu_reflector;
	G4ThreeVector position_tracker_THGEM1_Cu_reflector;
	G4ThreeVector position_Insulator_box;
	G4ThreeVector position_LAr_inner;
	G4ThreeVector position_LAr_outer;

	//
	G4ThreeVector positionSteelBox0;
	G4ThreeVector positionSteelBox1;
	G4ThreeVector positionSteelBox2;
	G4ThreeVector positionSteelBox3;

	//FieldWires
	G4ThreeVector position_FieldWire_bottom1;
	G4ThreeVector position_FieldWire_bottom2;
	G4ThreeVector position_FieldWire_bottom3;
	G4ThreeVector position_FieldWire_bottom4;
	G4ThreeVector position_FieldWire_top1;
	G4ThreeVector position_FieldWire_top2;
	G4ThreeVector position_FieldWire_top3;
	G4ThreeVector position_FieldWire_top4;
	G4ThreeVector position_Cathode;
	G4ThreeVector position_LArInactive;
	G4ThreeVector position_PMMA_bottom;
	G4ThreeVector position_Al_window_top;
	G4ThreeVector position_Al_window_bottom;
	G4ThreeVector position_CryogenicChamberBottom;
	G4ThreeVector position_ExternalColl;

	//bAlpha
	G4ThreeVector positionbAlpha;

	//Cd109
	G4ThreeVector position_Cd109;
	G4ThreeVector position_Cd109IsotopBoxHolder_top_hole;
	G4ThreeVector position_Cd109IsotopBoxHolder;
	G4ThreeVector position_Cd109IsotopBoxHolder_plug;
	G4ThreeVector position_Cd109InternalColl;
	G4ThreeVector position_Cd109BeFoil;

	//Cd109Detector
	G4ThreeVector position_Cd109Detector;

	//Cd109CuFoil
	G4ThreeVector position_Cd109CuFoil;

	//Cd109WolframPlug
	G4ThreeVector position_Cd109WolframPlug;

	//PMT
	G4ThreeVector position_PMT_0;
	G4ThreeVector position_PMT_1;
	G4ThreeVector position_PMT_2;
	G4ThreeVector position_PMT_3;

	//anode_grid
	G4ThreeVector position_anode_grid;


	//PMTAnodeGridTracker
	G4ThreeVector position_PMTAnodeGridTrackerGas_1;
	G4ThreeVector position_PMTAnodeGridTrackerGasInner_1;
	G4ThreeVector position_PMTAnodeGridTracker_1;
	G4ThreeVector position_PMTAnodeGridTracker_2;
	G4ThreeVector position_PMTAnodeGridTracker_3;
	G4ThreeVector position_PMTAnodeGridTrackerLiquid_1;
	G4ThreeVector position_PMTAnodeGridTrackerLiquidInner_1;

	//
	G4ThreeVector position_FieldTHGEM_1;
	G4ThreeVector position_FieldTHGEM_2;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
