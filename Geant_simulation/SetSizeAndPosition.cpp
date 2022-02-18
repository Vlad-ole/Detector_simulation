#include "DetectorConstruction.hh"
#include <G4OpticalSurface.hh>
#include <G4SystemOfUnits.hh> // this has appeared in GEANT4_10
#include "ReadConstants.h"
#include "Singleton.h"

void DetectorConstruction::SetSizeAndPosition()
{
	HalfWorldLength = 17 * cm;

	//PMTs
	radius_PMT = 45 * mm / 2.0;
	z_size_PMT = 5 * mm /*1 * um*/;
	x_pos_PMT = 152 * mm / 2.0 + z_size_PMT / 2;
	y_pos_PMT = x_pos_PMT;
	z_pos_PMT = 27.2 * mm + 63 * mm / 2.0;

	//anode wire
	radius_wire = 100 / 2.0 * um;//you can understand this from photo
	length_wire = 60 * mm /* 108*mm future case*/;  //60*mm /*real case*/;
	step_wire = 1 * mm;
	N_wire = length_wire / step_wire - 1 /* 107 future case*/;

	//interface wire
	radius_Interface_wire = 100 / 2.0 * um;//from Chegodaev
	length_Interface_wire = 100 * mm; // active region
	step_Interface_wire = 1 * mm; //from Chegodaev
	N_Interface_wire = length_Interface_wire / step_wire - 1;

	//Anode_grid
	thickness_anode_grid = 0.5 * mm;
	size_anode_grid = /*130 (from Ekaterina)*/ /*124*mm*/ 127*mm ;//see Download:\DetectorPhotos\2021\THGEM_Electroconnect
	size_anode_grid_hole = length_wire /*future case*/;  //60 * mm /*real case*/;
	z_anode_grid_bottom = 78.2 * mm /*78.2 in case of one THGEM*/ /*82.7*mm in case of two THGEM*/;
	double z_anode_grid_center = z_anode_grid_bottom + thickness_anode_grid / 2.0;

	


	//PMMA plate
	x_size_PMMA_plate = size_anode_grid;
	y_size_PMMA_plate = size_anode_grid;
	z_size_PMMA_plate = 1.5 * mm;
	double z_PMMA_plate_center = z_anode_grid_center + thickness_anode_grid / 2.0 + z_size_PMMA_plate / 2.0;

	//SiPMs
	Nx_SiPMs = 11;
	Ny_SiPMs = 11;
	thickness_SiPM = 1 * nm;
	size_SiPM = 6.0 * mm;
	chamberSpacing = 10 * mm;
	double z_SiPM_bottom = z_anode_grid_bottom + thickness_anode_grid + z_size_PMMA_plate + (0.1*mm /*small gap between PMMA and SiPM*/) /* 85.7*mm in case of two THGEM*/;
	double z_SiPM_center = z_SiPM_bottom + thickness_SiPM / 2.0;
	z_size_SiPMFR4 = 2*mm;


	cout << "z_SiPM_bottom = " << z_SiPM_bottom << endl;

	//tracker SiPM
	x_size_tracker = Nx_SiPMs * chamberSpacing + size_SiPM / 2.0;
	y_size_tracker = Ny_SiPMs * chamberSpacing + size_SiPM / 2.0;
	z_size_tracker = 0.1 * mm;

	//tracker Anode_grid
	x_size_tracker_anode_grid = length_wire;
	y_size_tracker_anode_grid = x_size_tracker_anode_grid;
	z_size_tracker_anode_grid = thickness_anode_grid;

	//tracker THGEM2 (active region with holes)
	x_size_tracker_THGEM2 = 100 * mm;
	y_size_tracker_THGEM2 = 100 * mm;
	z_size_tracker_THGEM2 = g()->width_THGEM1 /*0.4*/ /*500 * um*/;
	double z_tracker_THGEM2_center = 77.2 * mm + z_size_tracker_THGEM2 / 2.0;

	//solid_tracker_THGEM_Cu_reflector
	z_size_tracker_THGEM_Cu_reflector = z_size_tracker_THGEM2 / 10.0;

	//Interface_grid
	x_size_tracker_Interface_grid = x_size_tracker_THGEM2;
	y_size_tracker_Interface_grid = y_size_tracker_THGEM2;
	x_size_Interface_grid_substrate = size_anode_grid;
	y_size_Interface_grid_substrate = size_anode_grid;
	z_size_Interface_grid_substrate = 5 * mm;
	


	//THGEM0
	//const double z_THGEM0_bottom = 50.2 * mm;

	//THGEM_without_holes
	x_size_THGEM_without_holes = size_anode_grid;
	y_size_THGEM_without_holes = size_anode_grid;	
	z_size_THGEM_without_holes = z_size_tracker_THGEM2;


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
	x_size_Insulator_box_inner = 143 * mm;
	y_size_Insulator_box_inner = x_size_Insulator_box_inner;
	thickness_Insulator_box = 2 * mm;
	x_size_Insulator_box_outer = x_size_Insulator_box_inner + thickness_Insulator_box * 2;
	y_size_Insulator_box_outer = x_size_Insulator_box_outer;
	z_size_Insulator_box = 150 * mm;
	z_Insulator_box_center = z_size_Insulator_box / 2.0;

	

	//WLS
	/*const double radius_WLS = 70 * mm / 2.0;
	const double z_size_WLS = 100 * um;
	const double x_pos_WLS = 152 * mm / 2.0 + z_size_PMT / 2;
	const double y_pos_WLS = x_pos_WLS;
	const double z_pos_WLS = z_pos_PMT;*/

	//LAr_inner
	x_size_LAr_inner = x_size_Insulator_box_inner;
	y_size_LAr_inner = y_size_Insulator_box_inner;
	z_size_LAr_inner = g()->z_bottom_THGEM0 + z_size_tracker_THGEM2 + /*4 * mm*/ (22 - g()->EL_gap_thickness);

	//LArOuter
	x_size_LAr_outer_in = x_size_Insulator_box_outer;
	y_size_LAr_outer_in = y_size_Insulator_box_outer;
	x_size_LAr_outer_out = 152 * mm;
	y_size_LAr_outer_out = x_size_LAr_outer_out;
	z_size_LAr_outer = z_size_LAr_inner;

	//FieldTHGEM
	x_size_FieldTHGEM = size_anode_grid;
	y_size_FieldTHGEM = size_anode_grid;
	z_size_FieldTHGEM = z_size_tracker_THGEM2;
	z_center_FieldTHGEM_1 = 18.2*mm + z_size_FieldTHGEM / 2;
	z_center_FieldTHGEM_2 = 34.2*mm + z_size_FieldTHGEM / 2;
	hole_size_FieldTHGEM = 88 * mm;

	//FieldWire
	radius_FieldWire = 1.5 * mm / 2.0;
	length_FieldWire = 95 * mm;
	x_pos_FieldWire = x_size_tracker_THGEM2 / 2.0;
	z_pos_FieldWire_bottom = 20 /*18.2*mm - radius_FieldWire*/;//see 210415 1618834738620-1618834738657 photos
	z_pos_FieldWire_top = 33 /*34.2*mm + radius_FieldWire*/;//see 210415 1618834738620-1618834738657 photos

	//Cathode
	x_size_Cathode = x_size_LAr_outer_out;
	y_size_Cathode = y_size_LAr_outer_out;
	z_size_Cathode = 0.5 * mm;

	//LArInactive
	x_size_LArInactive = x_size_LAr_outer_out;
	y_size_LArInactive = y_size_LAr_outer_out;
	z_size_LArInactive = 2 * mm;

	//PMMA_bottom
	x_size_PMMA_bottom = x_size_LAr_outer_out;
	y_size_PMMA_bottom = y_size_LAr_outer_out;
	z_size_PMMA_bottom = 3 * mm;
	PMMA_bottom_center = -z_size_Cathode - z_size_LArInactive - z_size_PMMA_bottom / 2.0;

	//Al_window
	diameter_size_Al_window = 50 * mm;
	z_size_Al_window = 1.0 * mm;
	z_space_Al_window = 21 * mm;
	Al_window_top_center = PMMA_bottom_center - z_size_PMMA_bottom / 2.0 - z_size_Al_window / 2.0;
	Al_window_bottom_center = Al_window_top_center - z_size_Al_window / 2.0 - z_space_Al_window - z_size_Al_window / 2.0;

	//CryogenicChamberBottom
	diameter_size_internal_CryogenicChamberBottom = diameter_size_Al_window;
	diameter_size_external_CryogenicChamberBottom = x_size_LAr_inner * sqrt(2);
	z_size_CryogenicChamberBottom = 10 * mm;
	CryogenicChamberBottom_center = PMMA_bottom_center - z_size_PMMA_bottom / 2.0 - z_size_CryogenicChamberBottom / 2.0;

	//ExternalColl
	diameter_ExternalColl = diameter_size_Al_window;
	z_size_ExternalColl = 12 * mm;
	ExternalColl_center = Al_window_bottom_center - z_size_Al_window / 2.0 - z_size_ExternalColl / 2.0;
	ExternalColl_bottom = ExternalColl_center - z_size_ExternalColl / 2.0;
	cout << "ExternalColl_bottom = " << ExternalColl_bottom << endl;

	//Cd109IsotopBoxHolder
	z_size_Cd109IsotopBoxHolder_top_hole = 2 * mm;
	z_size_Cd109IsotopBoxHolder_middle_hole = 3 * mm;
	z_size_Cd109IsotopBoxHolder_bottom_hole = 12 * mm;
	z_size_Cd109IsotopBoxHolder = z_size_Cd109IsotopBoxHolder_top_hole + z_size_Cd109IsotopBoxHolder_middle_hole + z_size_Cd109IsotopBoxHolder_bottom_hole;//summ = 17*mm
	diameter_size_Cd109IsotopBoxHolder_top_hole = 7 * mm;
	diameter_size_Cd109IsotopBoxHolder = 14.3 * mm;
	Cd109IsotopBoxHolder_top_hole_bottom = Cd109ExternalBox_bottom + z_size_Cd109IsotopBoxHolder_bottom_hole + z_size_Cd109IsotopBoxHolder_middle_hole;
	Cd109IsotopBoxHolder_top_hole_center = Cd109IsotopBoxHolder_top_hole_bottom + z_size_Cd109IsotopBoxHolder_top_hole / 2.0;
	Cd109IsotopBoxHolder_center = Cd109ExternalBox_bottom + z_size_Cd109IsotopBoxHolder / 2.0;

	//Cd109
	diameter_size_Cd109 = 1 * mm;
	z_size_Cd109 = 0.1 * mm;
	Cd109_center = Cd109IsotopBoxHolder_top_hole_bottom - z_size_Cd109 / 2.0;
	g()->z_source_gamma = Cd109_center;
	g()->radius_source_gamma = diameter_size_Cd109 / 2.0;
	cout << "Cd109_center" << Cd109_center << endl;
	Cd109IsotopBoxHolder_plug_center = Cd109ExternalBox_bottom + z_size_Cd109IsotopBoxHolder_bottom_hole / 2.0 - z_size_Cd109;

	//Cd109BeFoil
	diameter_size_Cd109BeFoil = 3 * mm;
	z_size_Cd109BeFoil = 0.1 * mm;
	Cd109BeFoil_center = Cd109_center + z_size_Cd109 / 2.0 + z_size_Cd109BeFoil / 2.0;

	//Cd109WolframPlug
	diameter_size_Cd109WolframPlug = 3 * mm;
	z_size_Cd109WolframPlug = 3 * mm;
	Cd109WolframPlug_center = Cd109_center - z_size_Cd109 / 2.0 - z_size_Cd109WolframPlug / 2.0;

	//Cd109InternalColl
	diameter_size_internal_Cd109InternalColl = 2 * mm;
	diameter_size_external_Cd109InternalColl = 14.5 * mm;
	z_size_Cd109InternalColl = 10 * mm;
	Cd109InternalColl_center = Cd109ExternalBox_bottom + z_size_Cd109IsotopBoxHolder + z_size_Cd109InternalColl / 2.0;

	//Cd109CuFoil
	x_size_Cd109CuFoil = 16 * mm;
	y_size_Cd109CuFoil = x_size_Cd109CuFoil;
	z_size_Cd109CuFoil = 0.165 * mm;
	Cd109CuFoil_center = Cd109ExternalBox_bottom + z_size_Cd109IsotopBoxHolder + z_size_Cd109CuFoil / 2.0;

	//bCd109Detector
	x_size_Cd109Detector = 2 * mm;
	y_size_Cd109Detector = 10 * mm;
	z_size_Cd109Detector = 2 * mm;
	Cd109Detector_center = Cd109CuFoil_center + z_size_Cd109CuFoil / 2.0 + z_size_Cd109Detector / 2.0;

	if (thickness_anode_grid < 2 * radius_wire)
	{
		cout << "error: thickness_anode_grid < 2*radius_wire" << endl;
		system("pause");
	}

	//PMTGridWire
	PMTGridWireRadius = /*1500 test_value*/ 150 /*200*/ / 2.0 * um;
	PMTGridWirePitch = /*5 test_value*/ 1.2 * mm;

	//PMTAnodeGridTracker
	PMTAnodeGridTrackerThickness = PMTGridWireRadius * 2;
	PMTAnodeGridTrackerGasYSize = 50 * mm;
	PMTAnodeGridTrackerGasXSize = radius_PMT - (z_size_LAr_inner - z_pos_PMT);
	cout << "z_size_LAr_inner = " << z_size_LAr_inner << endl;
	cout << "z_pos_PMT = " << z_pos_PMT << endl;
	cout << "PMTAnodeGridTrackerGasXSize = " << PMTAnodeGridTrackerGasXSize << endl;
	PMTAnodeGridTrackerZbottom = /*150*mm*/ z_pos_PMT;
	PMTAnodeGridNCellsGas = PMTAnodeGridTrackerGasXSize / PMTGridWirePitch;
	PMTAnodeGridNCellsGasInner = PMTAnodeGridTrackerGasYSize / PMTGridWirePitch;

	PMTAnodeGridTrackerLiquidXSize = 2*radius_PMT - PMTAnodeGridTrackerGasXSize;
	PMTAnodeGridTrackerLiquidYSize = PMTAnodeGridTrackerGasYSize;
	PMTAnodeGridNCellsLiquid = PMTAnodeGridTrackerLiquidXSize / PMTGridWirePitch;
	PMTAnodeGridNCellsLiquidInner = PMTAnodeGridTrackerLiquidYSize / PMTGridWirePitch;


	//SteelBox
	xSizeSteelBox = 3;
	ySizeSteelBox = x_size_LAr_outer_out;
	zSizeSteelBox = 70;

	//Alpha
	radiusAlphaFull = 12;
	z_size_Alpha = 2;

	//TPB
	radiusTPB = 35;
	z_size_TPB = 0.2;


	//--------------------------------------------------------------------------------
	//определение взаимного расположения объектов
	
	position_SingleTHGEMHole = G4ThreeVector(g()->xyz_position_SingleTHGEMHole, g()->xyz_position_SingleTHGEMHole, g()->xyz_position_SingleTHGEMHole);

	position_anode_grid = G4ThreeVector(0, 0, z_anode_grid_center);
	positionTracker = G4ThreeVector(0, 0, z_SiPM_center);
	position_PMMA_plate = G4ThreeVector(0, 0, z_PMMA_plate_center);
	position_SiPMFR4 = G4ThreeVector(0, 0, z_SiPM_center + z_size_tracker /2.0 + z_size_SiPMFR4/2.0);

	position_tracker_THGEM2 = G4ThreeVector(0, 0, z_tracker_THGEM2_center);
	position_tracker_THGEM1 = G4ThreeVector(0, 0, g()->z_bottom_THGEM1 + z_size_tracker_THGEM2 / 2.0);
	position_tracker_THGEM0 = G4ThreeVector(0, 0, g()->z_bottom_THGEM0 + z_size_tracker_THGEM2 / 2.0 - z_size_LAr_inner / 2.0);

	position_tracker_THGEM0_Cu_reflector = G4ThreeVector(0, 0, 0);
	position_tracker_THGEM1_Cu_reflector = G4ThreeVector(0, 0, 0);

	position_Insulator_box = G4ThreeVector(0, 0, z_Insulator_box_center);

	position_LAr_inner = G4ThreeVector(0, 0, z_size_LAr_inner / 2.0);
	position_LAr_outer = G4ThreeVector(0, 0, z_size_LAr_inner / 2.0);

	//FieldWires
	position_FieldWire_bottom1 = G4ThreeVector(x_pos_FieldWire, 0, z_pos_FieldWire_bottom - z_size_LAr_inner / 2.0);
	position_FieldWire_bottom2 = G4ThreeVector(-x_pos_FieldWire, 0, z_pos_FieldWire_bottom - z_size_LAr_inner / 2.0);
	position_FieldWire_bottom3 = G4ThreeVector(0, x_pos_FieldWire, z_pos_FieldWire_bottom - z_size_LAr_inner / 2.0);
	position_FieldWire_bottom4 = G4ThreeVector(0, -x_pos_FieldWire, z_pos_FieldWire_bottom - z_size_LAr_inner / 2.0);
	position_FieldWire_top1 = G4ThreeVector(x_pos_FieldWire, 0, z_pos_FieldWire_top - z_size_LAr_inner / 2.0);
	position_FieldWire_top2 = G4ThreeVector(-x_pos_FieldWire, 0, z_pos_FieldWire_top - z_size_LAr_inner / 2.0);
	position_FieldWire_top3 = G4ThreeVector(0, x_pos_FieldWire, z_pos_FieldWire_top - z_size_LAr_inner / 2.0);
	position_FieldWire_top4 = G4ThreeVector(0, -x_pos_FieldWire, z_pos_FieldWire_top - z_size_LAr_inner / 2.0);
	
	position_Cathode = G4ThreeVector(0, 0, -z_size_Cathode / 2.0);
	position_LArInactive = G4ThreeVector(0, 0, -z_size_Cathode - z_size_LArInactive / 2.0);
	position_PMMA_bottom = G4ThreeVector(0, 0, PMMA_bottom_center);
	position_Al_window_top = G4ThreeVector(0, 0, Al_window_top_center);
	position_Al_window_bottom = G4ThreeVector(0, 0, Al_window_bottom_center);
	position_CryogenicChamberBottom = G4ThreeVector(0, 0, CryogenicChamberBottom_center);
	position_ExternalColl = G4ThreeVector(0, 0, ExternalColl_center);

	//TPB
	double x_pos_TPB = x_size_LAr_inner / 2.0 - z_size_TPB/2.0;
	double z_pos_TPB = -z_size_LAr_inner/2.0 + z_pos_PMT;
	position_TPB_0 = G4ThreeVector(-x_pos_TPB, 0, z_pos_TPB);
	position_TPB_1 = G4ThreeVector(x_pos_TPB, 0, z_pos_TPB);
	position_TPB_2 = G4ThreeVector(0, -x_pos_TPB, z_pos_TPB);
	position_TPB_3 = G4ThreeVector(0, x_pos_TPB, z_pos_TPB);

	//Cd109
	position_Cd109 = G4ThreeVector(0, 0, Cd109_center);
	position_Cd109IsotopBoxHolder_top_hole = G4ThreeVector(0, 0, Cd109IsotopBoxHolder_top_hole_center);
	position_Cd109IsotopBoxHolder = G4ThreeVector(0, 0, Cd109IsotopBoxHolder_center);
	position_Cd109IsotopBoxHolder_plug = G4ThreeVector(0, 0, Cd109IsotopBoxHolder_plug_center);
	position_Cd109InternalColl = G4ThreeVector(0, 0, Cd109InternalColl_center);
	position_Cd109BeFoil = G4ThreeVector(0, 0, Cd109BeFoil_center);

	//Cd109Detector
	position_Cd109Detector = G4ThreeVector(0, 0, Cd109Detector_center);

	//Cd109CuFoil
	position_Cd109CuFoil = G4ThreeVector(0, 0, Cd109CuFoil_center);

	//Cd109WolframPlug
	position_Cd109WolframPlug = G4ThreeVector(0, 0, Cd109WolframPlug_center);

	//PMT
	position_PMT_0 = G4ThreeVector(-x_pos_PMT, 0, z_pos_PMT);
	position_PMT_1 = G4ThreeVector(x_pos_PMT, 0, z_pos_PMT);
	position_PMT_2 = G4ThreeVector(0, -y_pos_PMT, z_pos_PMT);
	position_PMT_3 = G4ThreeVector(0, y_pos_PMT, z_pos_PMT);

	//anode_grid
	position_anode_grid = G4ThreeVector(0, 0, z_anode_grid_center);
	
	//PMTAnodeGridTracker
	const double x_pos_PMTAnodeGridTracker = (x_pos_PMT - z_size_PMT / 2.0 - PMTAnodeGridTrackerThickness / 2.0);
	const double y_pos_PMTAnodeGridTracker = 0;
	const double z_pos_PMTAnodeGridTracker = PMTAnodeGridTrackerGasXSize /2.0 + z_size_LAr_inner;
	position_PMTAnodeGridTrackerGas_1 = G4ThreeVector(x_pos_PMTAnodeGridTracker, y_pos_PMTAnodeGridTracker, z_pos_PMTAnodeGridTracker);
	position_PMTAnodeGridTrackerGasInner_1 = G4ThreeVector(x_pos_PMTAnodeGridTracker - PMTAnodeGridTrackerThickness, y_pos_PMTAnodeGridTracker, z_pos_PMTAnodeGridTracker);
	
	const double x_pos_PMTAnodeGridTrackerLiquid = x_pos_PMTAnodeGridTracker;
	const double y_pos_PMTAnodeGridTrackerLiquid = 0;
	const double z_pos_PMTAnodeGridTrackerLiquid = z_size_LAr_inner - PMTAnodeGridTrackerLiquidXSize/2.0 - z_size_LAr_inner / 2.0;
	position_PMTAnodeGridTrackerLiquid_1 = G4ThreeVector(x_pos_PMTAnodeGridTrackerLiquid, y_pos_PMTAnodeGridTrackerLiquid, z_pos_PMTAnodeGridTrackerLiquid);
	position_PMTAnodeGridTrackerLiquidInner_1 = G4ThreeVector(x_pos_PMTAnodeGridTrackerLiquid - PMTAnodeGridTrackerThickness, y_pos_PMTAnodeGridTrackerLiquid, z_pos_PMTAnodeGridTrackerLiquid);

	position_FieldTHGEM_1 = G4ThreeVector(0, 0, z_center_FieldTHGEM_1 - z_size_LAr_inner / 2.0);
	position_FieldTHGEM_2 = G4ThreeVector(0, 0, z_center_FieldTHGEM_2 - z_size_LAr_inner / 2.0);

	//SteelBox0
	const double xPosSteelBox = x_pos_PMT - z_size_PMT / 2.0 + xSizeSteelBox/2.0;
	positionSteelBox0 = G4ThreeVector(-xPosSteelBox, 0, z_pos_PMT);
	positionSteelBox1 = G4ThreeVector(xPosSteelBox, 0, z_pos_PMT);
	positionSteelBox2 = G4ThreeVector(0, -xPosSteelBox, z_pos_PMT);
	positionSteelBox3 = G4ThreeVector(0, xPosSteelBox, z_pos_PMT);

	//positionbAlpha
	positionbAlpha = G4ThreeVector(0, 0, 0 - z_size_LAr_inner/2.0 + z_size_Alpha/2.0);
}