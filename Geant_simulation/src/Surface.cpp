#include "DetectorConstruction.hh"
#include <G4OpticalSurface.hh>
#include <G4SystemOfUnits.hh> // this has appeared in GEANT4_10
#include "ReadConstants.h"
#include "Singleton.h"

void DetectorConstruction::defineSurfaces()
{
	//*********************************************************************************
	//LUT model
	BGOPolishedAirTeflon = new G4OpticalSurface("BGOPolishedAirTeflon", unified);
	BGOPolishedAirTeflon->SetType(dielectric_LUT);
	BGOPolishedAirTeflon->SetModel(LUT);
	BGOPolishedAirTeflon->SetFinish(polishedteflonair);

	BGOGroundAirTeflon = new G4OpticalSurface("BGOGroundAirTeflon", unified);
	BGOGroundAirTeflon->SetType(dielectric_LUT);
	BGOGroundAirTeflon->SetModel(LUT);
	BGOGroundAirTeflon->SetFinish(groundteflonair);

	Polished_Air_TiO2 = new G4OpticalSurface("Polished_Air_TiO2", unified);
	Polished_Air_TiO2->SetType(dielectric_LUT);
	Polished_Air_TiO2->SetModel(LUT);
	Polished_Air_TiO2->SetFinish(polishedtioair);

	Ground_Air_TiO2 = new G4OpticalSurface("Ground_Air_TiO2", unified);
	Ground_Air_TiO2->SetType(dielectric_LUT);
	Ground_Air_TiO2->SetModel(LUT);
	Ground_Air_TiO2->SetFinish(groundtioair);
	//*********************************************************************************



	//*********************************************************************************
	//unifed model
	G4double ener[2] = {.1*eV, 10.*eV};
	G4double specular_lobe[2] = {1.0, 1.0};
	G4double specular_spike[2] = {0.0, 0.0};
	G4double back_scatter[2] = {0.0, 0.0};

	//-------------------------------------------------------------------------------
	//polishedAir
	polishedAir = new G4OpticalSurface("polishedAir");
	polishedAir->SetModel(unified);
	polishedAir->SetType(dielectric_dielectric);	
	polishedAir->SetFinish(ground); // ground necessary even for polished surfaces to enable UNIFIED code
	polishedAir->SetSigmaAlpha(g()->SigmaAlpha_index * degree); // Janecek2010

	G4MaterialPropertiesTable* polishedAir_property = new G4MaterialPropertiesTable();
	//polishedAir_property->AddProperty("RINDEX", ener, teflon_rindex, 2);
	polishedAir_property->AddProperty("SPECULARLOBECONSTANT", ener, specular_lobe,2);
	polishedAir_property->AddProperty("SPECULARSPIKECONSTANT", ener, specular_spike,2);
	polishedAir_property->AddProperty("BACKSCATTERCONSTANT", ener, back_scatter,2);
	//polishedAir_property->AddProperty("REFLECTIVITY", ener, teflon_refl, 2);
	polishedAir->SetMaterialPropertiesTable(polishedAir_property);
	//-------------------------------------------------------------------------------





	//-------------------------------------------------------------------------------
	//FR4_unified
	FR4_unified = new G4OpticalSurface("FR4_unified");
	FR4_unified->SetType(dielectric_metal);
	FR4_unified->SetModel(unified);
	FR4_unified->SetFinish(groundair);
	FR4_unified->SetSigmaAlpha(10);//alpha in degrees, from 0 to 90.


	G4MaterialPropertiesTable *FR4_MaterialProperty = new G4MaterialPropertiesTable();
	//G4double FR4_Materialrefl[2] = { 0.05, 0.05 };//https://www.cetem.gov.br/images/congressos/2008/CAC00560008.pdf Specular Reflectance Data for Quartz and Some Epoxy Resins –	Implications for Digital Image Analysis Based on Reflected Light Optical Microscopy
	G4double FR4_Materialrefl[2] = { 0.3, 0.3 };
	G4double FR4_Materialeff[2] = { 0, 0 };
	 
	FR4_MaterialProperty->AddProperty("REFLECTIVITY", ener, FR4_Materialrefl, 2);
	FR4_MaterialProperty->AddProperty("EFFICIENCY", ener, FR4_Materialeff, 2);

	FR4_unified->SetMaterialPropertiesTable(FR4_MaterialProperty);
	//-------------------------------------------------------------------------------
		

	//-------------------------------------------------------------------------------
	//Anode_Wire (Bronze-Berillium)
	Anode_wire_unified = new G4OpticalSurface("Anode_wire_unified");
	Anode_wire_unified->SetType(dielectric_metal);
	Anode_wire_unified->SetModel(unified);
	Anode_wire_unified->SetFinish(polished);
	Anode_wire_unified->SetSigmaAlpha(1);//alpha in degrees, from 0 to 90.
	G4MaterialPropertiesTable *Anode_wire_MaterialProperty = new G4MaterialPropertiesTable();
	G4double Anode_wire_Materialrefl[2] = { 0.5, 0.5 };//approximatly https://nvlpubs.nist.gov/nistpubs/bulletin/07/nbsbulletinv7n2p197_A2b.pdf The Reflecting Power of Various Metals
	G4double Anode_wire_Materialeff[2] = { 0, 0 };
	Anode_wire_MaterialProperty->AddProperty("REFLECTIVITY", ener, Anode_wire_Materialrefl, 2);
	Anode_wire_MaterialProperty->AddProperty("EFFICIENCY", ener, Anode_wire_Materialeff, 2);

	Anode_wire_unified->SetMaterialPropertiesTable(Anode_wire_MaterialProperty);

	//-------------------------------------------------------------------------------


	
	
	//---------------------------------------------------------------------------
	//teflon
	

	G4double teflon_rindex[2] = {1.35, 1.35}; //wiki (Optical properties of materials for concentrator photovoltaic systems) [1.35-1.38]
	G4double teflon_refl[2] = {0.99, 0.99}; // Janecek2010

	teflon_unified = new G4OpticalSurface("polishedWhitePainted", unified);
	teflon_unified->SetType(dielectric_dielectric);
	teflon_unified->SetModel(unified);
	teflon_unified->SetFinish(groundbackpainted);
	teflon_unified->SetSigmaAlpha(0.0741 * degree); // Janecek2010

	G4MaterialPropertiesTable* teflon_unified_property = new G4MaterialPropertiesTable();
	teflon_unified_property->AddProperty("RINDEX", ener, teflon_rindex, 2);
	teflon_unified_property->AddProperty("SPECULARLOBECONSTANT", ener, specular_lobe,2);
	teflon_unified_property->AddProperty("SPECULARSPIKECONSTANT", ener, specular_spike,2);
	teflon_unified_property->AddProperty("BACKSCATTERCONSTANT", ener, back_scatter,2);
	teflon_unified_property->AddProperty("REFLECTIVITY", ener, teflon_refl, 2);
	teflon_unified->SetMaterialPropertiesTable(teflon_unified_property);
	//-------------------------------------------------------------------------------
	 
	
	
	//---------------------------------------------------------------------------
	//MgO
	ReadConstants *MgO_RINDEX = new ReadConstants(g()->string_MgO_RINDEX, 1*eV, 1);
	ReadConstants *MgO_REFLECTIVITY = new ReadConstants(g()->string_MgO_REFLECTIVITY, 1*eV, 1);

	MgO_unified = new G4OpticalSurface("polishedWhitePainted", unified);
	MgO_unified->SetType(dielectric_dielectric);
	MgO_unified->SetModel(unified);
	MgO_unified->SetFinish(groundbackpainted);
	MgO_unified->SetSigmaAlpha(1.3 * degree); // Janecek2010

	G4MaterialPropertiesTable* MgO_unified_property = new G4MaterialPropertiesTable();
	MgO_unified_property->AddProperty("RINDEX", MgO_RINDEX->get_x_array(), MgO_RINDEX->get_y_array(), MgO_RINDEX->get_array_size());
	MgO_unified_property->AddProperty("SPECULARLOBECONSTANT", ener, specular_lobe,2);
	MgO_unified_property->AddProperty("SPECULARSPIKECONSTANT", ener, specular_spike,2);
	MgO_unified_property->AddProperty("BACKSCATTERCONSTANT", ener, back_scatter,2);
	MgO_unified_property->AddProperty("REFLECTIVITY", MgO_REFLECTIVITY->get_x_array(), MgO_REFLECTIVITY->get_y_array(), MgO_REFLECTIVITY->get_array_size());
	MgO_unified->SetMaterialPropertiesTable(MgO_unified_property);
	//-------------------------------------------------------------------------------

	
	//---------------------------------------------------------------------------
	//TiO2
	ReadConstants *TiO2_RINDEX = new ReadConstants(g()->string_TiO2_RINDEX, 1*eV, 1);
	ReadConstants *TiO2_REFLECTIVITY = new ReadConstants(g()->string_TiO2_REFLECTIVITY, 1*eV, 1);

	TiO2_unified = new G4OpticalSurface("polishedWhitePainted", unified);
	TiO2_unified->SetType(dielectric_dielectric);
	TiO2_unified->SetModel(unified);
	TiO2_unified->SetFinish(groundbackpainted);
	TiO2_unified->SetSigmaAlpha(1.3 * degree); // Janecek2010

	G4MaterialPropertiesTable* TiO2_unified_property = new G4MaterialPropertiesTable();
	TiO2_unified_property->AddProperty("RINDEX", TiO2_RINDEX->get_x_array(), TiO2_RINDEX->get_y_array(), TiO2_RINDEX->get_array_size());
	TiO2_unified_property->AddProperty("SPECULARLOBECONSTANT", ener, specular_lobe,2);
	TiO2_unified_property->AddProperty("SPECULARSPIKECONSTANT", ener, specular_spike,2);
	TiO2_unified_property->AddProperty("BACKSCATTERCONSTANT", ener, back_scatter,2);
	TiO2_unified_property->AddProperty("REFLECTIVITY", TiO2_REFLECTIVITY->get_x_array(), TiO2_REFLECTIVITY->get_y_array(), TiO2_REFLECTIVITY->get_array_size());
	TiO2_unified->SetMaterialPropertiesTable(TiO2_unified_property);
	//------------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------------
	//описание поверхности стекла ФЭУ
	Glass_surface = new G4OpticalSurface("Glass_surface");
	Glass_surface->SetModel(unified);
	Glass_surface->SetType(dielectric_dielectric);	
	Glass_surface->SetFinish(ground); // ground necessary even for polished surfaces to enable UNIFIED code
	Glass_surface->SetSigmaAlpha(0.0 * degree); // Janecek2010

	G4MaterialPropertiesTable* Glass_surface_property = new G4MaterialPropertiesTable();
	//polishedAir_property->AddProperty("RINDEX", ener, teflon_rindex, 2);
	Glass_surface_property->AddProperty("SPECULARLOBECONSTANT", ener, specular_lobe,2);
	Glass_surface_property->AddProperty("SPECULARSPIKECONSTANT", ener, specular_spike,2);
	Glass_surface_property->AddProperty("BACKSCATTERCONSTANT", ener, back_scatter,2);
	//polishedAir_property->AddProperty("REFLECTIVITY", ener, teflon_refl, 2);
	Glass_surface->SetMaterialPropertiesTable(Glass_surface_property);
	//-------------------------------------------------------------------------------

	
	//-----------------------------------------------------------------------------
	// PMT cathode
	PMT_cathode = new G4OpticalSurface("PMT_cathode", unified);
	PMT_cathode->SetType(dielectric_metal);
	PMT_cathode->SetModel(unified);
	PMT_cathode->SetFinish(polished);
	PMT_cathode->SetSigmaAlpha(0.);

	

	PMT_cathodeMaterialProperty = new G4MaterialPropertiesTable();
	G4double cathoderefl[2] = {0, 0};
	//G4double cathodeeff[2] = {1, 1};
	G4double SiPMEeff[2] = { 1, 1 };

	ReadConstants *PMT_cathode_EFFICIENCY = new ReadConstants(g()->string_PMT_R6041_506MOD_EFFICIENCY, 1*eV, 1);
	//ReadConstants *Cathode_REFLECTIVITY = new ReadConstants(g()->string_Cathode_REFLECTIVITY, 1*eV, 1);

	

	
	//silicaCathodeMaterialProperty->AddProperty("REFLECTIVITY", Cathode_REFLECTIVITY->get_x_array(), Cathode_REFLECTIVITY->get_y_array(), Cathode_REFLECTIVITY->get_array_size());
	PMT_cathodeMaterialProperty->AddProperty("EFFICIENCY", PMT_cathode_EFFICIENCY->get_x_array(), PMT_cathode_EFFICIENCY->get_y_array(), PMT_cathode_EFFICIENCY->get_array_size());
	

	PMT_cathodeMaterialProperty->AddProperty("REFLECTIVITY", ener, cathoderefl, 2);
	//silicaCathodeMaterialProperty->AddProperty("EFFICIENCY", ener, SiPMEeff, 2);

	PMT_cathode->SetMaterialPropertiesTable(PMT_cathodeMaterialProperty);
	//--------------------------------------------------------------------------------





	//-----------------------------------------------------------------------------
	// SiPM_OpticalSurface
	SiPM_OpticalSurface = new G4OpticalSurface("SiPM_OpticalSurface", unified);
	SiPM_OpticalSurface->SetType(dielectric_metal);
	SiPM_OpticalSurface->SetModel(unified);
	SiPM_OpticalSurface->SetFinish(polished);
	SiPM_OpticalSurface->SetSigmaAlpha(0.);
	

	G4MaterialPropertiesTable* SiPM_MaterialProperty = new G4MaterialPropertiesTable();
	G4double SiPM_refl[2] = { 0, 0 };
	G4double cathodeeff[2] = { 1, 1 };

	ReadConstants *SiPM_EFFICIENCY = new ReadConstants(g()->string_SiPM_13360_6050pe_46V_EFFICIENCY, 1*eV, 1);
	//ReadConstants *Cathode_REFLECTIVITY = new ReadConstants(g()->string_Cathode_REFLECTIVITY, 1*eV, 1);


	//silicaCathodeMaterialProperty->AddProperty("REFLECTIVITY", Cathode_REFLECTIVITY->get_x_array(), Cathode_REFLECTIVITY->get_y_array(), Cathode_REFLECTIVITY->get_array_size());
	//SiPM_MaterialProperty->AddProperty("EFFICIENCY", SiPM_EFFICIENCY->get_x_array(), SiPM_EFFICIENCY->get_y_array(), SiPM_EFFICIENCY->get_array_size());


	SiPM_MaterialProperty->AddProperty("REFLECTIVITY", ener, SiPM_refl, 2);
	//SiPM_MaterialProperty->AddProperty("EFFICIENCY", ener, cathodeeff, 2);//dummy
	SiPM_MaterialProperty->AddProperty("EFFICIENCY", SiPM_EFFICIENCY->get_x_array(), SiPM_EFFICIENCY->get_y_array(), SiPM_EFFICIENCY->get_array_size());

	SiPM_OpticalSurface->SetMaterialPropertiesTable(SiPM_MaterialProperty);
	//--------------------------------------------------------------------------------








	//-----------------------------------------------------------------------------
	// описание поверхности поглотителя
	AbsorberMaterial = new G4OpticalSurface("Absorber", unified);
	AbsorberMaterial->SetType(dielectric_metal);
	AbsorberMaterial->SetModel(unified);
	AbsorberMaterial->SetFinish(polished);
	AbsorberMaterial->SetSigmaAlpha(0.);



	G4MaterialPropertiesTable *AbsorberMaterialProperty = new G4MaterialPropertiesTable();
	G4double AbsorberMaterialrefl[2] = { 0.0, 0.0 };
	G4double AbsorberMaterialeff[2] = { 0, 0 };
	
	AbsorberMaterialProperty->AddProperty("REFLECTIVITY", ener, AbsorberMaterialrefl, 2);
	AbsorberMaterialProperty->AddProperty("EFFICIENCY", ener, AbsorberMaterialeff, 2);

	AbsorberMaterial->SetMaterialPropertiesTable(AbsorberMaterialProperty);
	//--------------------------------------------------------------------------------



	//*********************************************************************************
}

void DetectorConstruction::ChangeSurface(double parametr)
{
	polishedAir->SetSigmaAlpha(parametr * degree);
}

void DetectorConstruction::ChangeCathRefl()
{
	G4double ener[2] = { .1*eV, 10.*eV };
	G4double cathoderefl[2] = { g()->CathRefl_index, g()->CathRefl_index };

	G4double factory_eff = 0.25;
	G4double cathodeeff[2] = { factory_eff / (1 - g()->CathRefl_index), factory_eff / (1 - g()->CathRefl_index) };
	
	PMT_cathodeMaterialProperty->AddProperty("REFLECTIVITY", ener, cathoderefl, 2);
	PMT_cathodeMaterialProperty->AddProperty("EFFICIENCY", ener, cathodeeff, 2);
}