#include "Singleton.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Singleton::Singleton()
{
	summ_number_of_photons = 0;

	//string solution_path = "C:\\diplom\\Geant_simulation\\";
	solution_path = "D:\\git_repositories\\Detector_simulation\\Geant_simulation\\";

	//����������� ���� ������
	string path_out = solution_path + "experiment\\";
	path_read = solution_path + "data\\";

	// ����� �� ������
	file_num_of_photons.open(path_out + "num_of_photons.dat");
	file_num_of_photons_in_event_SiPM.open(path_out + "num_of_photons_in_event_SiPM.dat");
	file_coord_from_SiPM.open(path_out + "coord_from_SiPM.dat");
	file_real_position_of_source.open(path_out + "real_position_of_source.dat");	
	file_total_num_of_reg_photons_SiPM.open(path_out + "total_num_of_reg_photons_SiPM.dat");
	file_PMT1_angles.open(path_out + "PMT1_angles.dat");
	file_num_of_photons_PMT.open(path_out + "num_of_photons_PMT.dat");
	file_PE_map_SiPM.open(path_out + "PE_map_SiPM.dat");
	file_LRF_SiPM.open(path_out + "LRF_SiPM.dat");
	file_num_of_reg_photons.open(path_out + "num_of_reg_photons.dat");
	file_ph_coll.open(path_out + "ph_coll.dat");
	file_xy.open(path_out + "xy.dat");
	file_emitted_energy.open(path_out + "emitted_energy.dat");
	file_energy.open(path_out + "energy.dat");
	file_run_lc.open(path_out + "run_lc.dat");
	file_boundary_process.open(path_out + "boundary_process.dat");
	file_xyz_source.open(path_out + "xyz_source.dat");
	file_gamma_event_E_depos.open(path_out + "gamma_event_E_depos.dat");
	file_gamma_step_E_depos.open(path_out + "gamma_step_E_depos.dat");

	// ����� �� ������
	string_pmma_absorption_length = path_read + "absorption_length\\PMMA_absorption_length_eV_mm.dat";
	string_pmma_rindex = path_read + "refractive_index\\PMMA_rindex_eV_1.dat";

	string_pmma_uv_absorption_length = path_read + "absorption_length\\PMMA_VU_absorption_length_eV_mm.dat";

	string_lyso_ce_energies = path_read + "energy_spectrum\\lyso_ce_energies_eV_1.dat";
	string_lyso_ce_rindex = path_read + "refractive_index\\lyso_ce_rindex_eV_1.dat";	
	string_lyso_ce_absorption_length = path_read + "absorption_length\\lyso_ce_absorption_length_eV_mm.dat";

	string_LuYAG_Pr_energies = path_read + "energy_spectrum\\luyag_pr_energies_eV_1.dat";
	string_LuYAG_Pr_rindex = path_read + "refractive_index\\luyag_pr_RINDEX_eV_1.dat";
	string_LuYAG_Pr_absorption_length = path_read  + "absorption_length\\luyag_pr_absorption_length_eV_mm.dat";

	string_LGSO_ce_energies = path_read + "energy_spectrum\\lgso_ce_energies_eV_1.dat";
	string_LGSO_ce_rindex = path_read + "refractive_index\\lgso_ce_rindex_eV_1.dat";	
	string_LGSO_ce_absorption_length = path_read + "absorption_length\\lgso_ce_absorption_length_eV_mm.dat";

	string_LFS_3_energies = path_read + "energy_spectrum\\LFS_3_energies_eV_1.dat";
	string_LFS_3_rindex = path_read + "refractive_index\\LFS_3_rindex_eV_1.dat";	
	string_LFS_3_absorption_length = path_read + "absorption_length\\LFS_3_absorption_length_eV_mm.dat";

	string_YAP_Ce_energies = path_read + "energy_spectrum\\YAP_Ce_energies_eV_1.dat";
	string_YAP_Ce_rindex = path_read + "refractive_index\\YAP_Ce_rindex_eV_1.dat";	
	string_YAP_Ce_absorption_length = path_read + "absorption_length\\YAP_Ce_absorption_length_eV_mm.dat";
	
	string_polydimethylsiloxane_RINDEX =  path_read + "refractive_index\\polydimethylsiloxane_RINDEX_eV_1.dat";
	string_polydimethylsiloxane_absorption_length = path_read + "absorption_length\\polydimethylsiloxane_absorption_length_eV_mm.dat";

	string_BorosilicateGlass_RINDEX = path_read + "refractive_index\\Borosilicate_N-BK7_RINDEX_eV_1.dat";
	string_BorosilicateGlass_ABSLENGTH = path_read + "absorption_length\\BorosilicateGlass_AbsorptionLength_eV_mm.dat";

	//string_silicaCathodeMaterial_EFFICIENCY = path_read + "detector_efficiency\\pmt_r10233(big).dat";
	string_PMT_R6041_506MOD_EFFICIENCY = path_read + "detector_efficiency\\PMT_R6041_506MOD.dat";
	string_SiPM_13360_6050pe_46V_EFFICIENCY = path_read + "detector_efficiency\\SiPM_s13360-6050pe_46V.dat";
	string_SiPM_13360_6050pe_48V_EFFICIENCY = path_read + "detector_efficiency\\SiPM_s13360-6050pe_48V.dat";

	string_MgO_REFLECTIVITY = path_read + "reflectivity\\MgO_REFLECTIVITY_eV_1.dat";
	string_MgO_RINDEX = path_read + "refractive_index\\MgO_RINDEX_eV_1.dat";

	string_TiO2_REFLECTIVITY = path_read + "reflectivity\\TiO2_REFLECTIVITY_eV_1.dat";
	string_TiO2_RINDEX = path_read + "refractive_index\\TiO2_RINDEX_eV_1.dat";

	//
	string_Cathode_REFLECTIVITY = path_read + "reflectivity\\Cathode_Motta_REFLECTIVITY_eV_1.dat";


	string_GAr_86K_1atm_avalanche_scint_NIR = path_read + "energy_spectrum\\GAr_86K_1atm_avalanche_scint_NIR.dat";
	string_LAr_T_Heindl_2011 = path_read + "energy_spectrum\\T_Heindl_2011.dat";
	
}

Singleton* Singleton::get() 
{

	if(Singleton::single==NULL)
	{
		single = new Singleton();
	}

	return single;

}
