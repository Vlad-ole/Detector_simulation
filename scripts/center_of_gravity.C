void Reconstruct()
{
	char* file_in_name = "D:\\git_repositories\\Detector_simulation\\Geant_simulation\\experiment\\num_of_photons_SiPM.dat";
	FILE* file_in = fopen(file_in_name,"r");
	
	if(file_in == NULL)
	{
		cout << "error" << endl;
		return 0;
	}
	else
	{
		cout << file_in_name << " is opened" << endl;
	}
	
	vector<int> N_reg_v;
	vector<double> xpos_v;	
	vector<double> ypos_v;		
	
	while ( !feof(file_in) )
	{ 
		int id;
		double x;
		double y;
		int N_reg;
		
		fscanf(file_in,"%d %lf %lf %d\n", &id, &x, &y, &N_reg);		
		xpos_v.push_back(x);
		ypos_v.push_back(y);
		N_reg_v.push_back(N_reg);
	}
	
	double x_mean = 0;
	double y_mean = 0;
	double q_mean = 0;
	
	for(int i = 0; i < N_reg_v.size(); i++)
	{
		x_mean += N_reg_v[i] * xpos_v[i];
		y_mean += N_reg_v[i] * ypos_v[i];
		q_mean += N_reg_v[i];
	}
	
	x_mean /= q_mean;
	y_mean /= q_mean;
	
	cout << "x_mean = " << x_mean << " \t y_mean = " << y_mean << endl;
}