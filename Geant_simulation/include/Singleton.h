#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Hits.h"
using namespace std;
#define g() (Singleton::get())

class Singleton
{

private:

	static Singleton *single;
	Singleton();

public:

	static Singleton* get(); 
	
	ofstream file_num_of_photons;
	ofstream file_num_of_photons_SiPM;
	ofstream file_coord_from_SiPM;
	ofstream file_real_position_of_source;
	ofstream file_total_num_of_reg_photons_SiPM;
	ofstream file_num_of_photons_PMT;
	ofstream file_num_of_reg_photons;
	ofstream file_ph_coll;
	ofstream file_xy;
	ofstream file_energy;
	ofstream file_run_lc;
	ofstream file_boundary_process;
	//gamma
	ofstream file_gamma_event_E_depos;

	
	string solution_path;
	string path_read;

	//LYSO:Ce
	string string_lyso_ce_energies;
	string string_lyso_ce_rindex;
	string string_lyso_ce_absorption_length;

	//LuYAG:Pr
	string string_LuYAG_Pr_energies;
	string string_LuYAG_Pr_rindex;
	string string_LuYAG_Pr_absorption_length;

	//LGSO:Ce
	string string_LGSO_ce_energies;
	string string_LGSO_ce_rindex;
	string string_LGSO_ce_absorption_length;

	//YAP:Ce
	string string_YAP_Ce_energies;
	string string_YAP_Ce_rindex;
	string string_YAP_Ce_absorption_length;

	//LFS-3
	string string_LFS_3_energies;
	string string_LFS_3_rindex;
	string string_LFS_3_absorption_length;
	

	//grease
	string string_polydimethylsiloxane_RINDEX;
	string string_polydimethylsiloxane_absorption_length;

	//glass
	string string_BorosilicateGlass_RINDEX;
	string string_BorosilicateGlass_ABSLENGTH;

	//cathode
	string string_silicaCathodeMaterial_EFFICIENCY;

	//MgO
	string string_MgO_REFLECTIVITY;
	string string_MgO_RINDEX;

	//TiO2
	string string_TiO2_REFLECTIVITY;
	string string_TiO2_RINDEX;

	//Cathode
	string string_Cathode_REFLECTIVITY;

	//SiPM
	string string_SiPM_EFFICIENCY;

	string string_GAr_86K_1atm_avalanche_scint_NIR;
	
	//
	double summ_number_of_photons;
	
	vector<double> LightCollection;
		
	int abs_index;
	
	double SigmaAlpha_index;
	double CathRefl_index;

	int NumberOfReflections;
	int NumberOfBornPhotons;
	int NumberOfRegPhotons;

	Hits* SiPM_hits;
	Hits* PMT_hits;

	//x,y,z coord of source
	double x_source;
	double y_source;
	double z_source;
	double x_source_gamma;
	double y_source_gamma;
	double z_source_gamma;
	double radius_source_gamma;

	bool is_optical_gamma;

	double l_x;
	double l_L;
	double h_s;
	double h_c;
};