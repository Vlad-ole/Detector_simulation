#include <iostream>
using namespace std;

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "Singleton.h"

#include <G4SystemOfUnits.hh> // this has appeared in GEANT4_10

//set angle distribution
//#define DIRECT_INCIDENCE
#define TOP_HEMISPHERE
//#define SPHERE_4PI
//#define ANGLE_Cd_COLL6mm

//set (x,y,z) position
//#define CENTRAL_INCIDENCE
//#define GEM_HOLE
#define EL_GAP 
//#define CIRCLE_EL_GAP
//#define CIRCLE_Cd


void PrimaryGeneratorAction::CommonPart()
{
	G4int n_particle = 1;
	particleGun = new G4ParticleGun(n_particle);

	// default particle kinematic
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;

	G4ParticleDefinition* particle;
	if(g()->is_optical_gamma)
		particle = particleTable->FindParticle(particleName = /*"gamma"*/ "opticalphoton");
	else
		particle = particleTable->FindParticle(particleName = "gamma" /*"opticalphoton"*/);

	particleGun->SetParticleDefinition(particle);
}

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
	CommonPart();
	xrType = MONO;
	particle_counter = 0;
}

PrimaryGeneratorAction::PrimaryGeneratorAction(const char* fname, bool IsDiscrete)
{
	CommonPart();

	if (!IsDiscrete)
	{


		xrType = SPECTER_continuous;

		char   buffer[256];
		char   seps[] = " \t;";	  //separatoren

		nMaxDataLines = 0;
		dMaxWeight = 0; //!!! // dMaxWeight  = 1;


		cout << "Read Specter: " << fname << endl;
		FILE* in = fopen(fname, "r");
		if (in == NULL)
		{
			cout << "Specter of X-Tube not loaded!!! " << fname << endl;
			return;
		}

		while (!feof(in) && (nMaxDataLines < MAX_SPECTER_DATALINES))
		{
			fgets(buffer, sizeof(buffer) - 1, in);
			cout << "Read of " << nMaxDataLines << " line: " << buffer << endl;

			if (strlen(buffer) > 2)
			{
				if (!strstr(buffer, "//"))
				{
					dEnergy[nMaxDataLines] = atoi(strtok(buffer, seps));
					dWeight[nMaxDataLines] = atof(strtok(NULL, seps));

					if (dWeight[nMaxDataLines] > dMaxWeight)
						dMaxWeight = dWeight[nMaxDataLines];

					nMaxDataLines++;
				}
			}
		}
		cout << "Specter " << fname << "loaded! There are " << nMaxDataLines << " datalines." << endl;

		fclose(in);

	}
	else
	{
		xrType = SPECTER_discrete;
		FILE* in = fopen(fname, "r");
		if (in == NULL)
		{
			cout << "Specter not loaded!!! " << fname << endl;
			return;
		}

		double x, y;
		while (!feof(in))
		{
			fscanf(in, "%lf %lf\n", &x, &y);
			Ev.push_back(x);
			pv.push_back(y);
		}

		pv_edge.resize(Ev.size());

		double summ = 0;
		for (int i = 0; i < Ev.size(); i++)
		{
			summ += pv[i];
			pv_edge[i] = summ;
		}
	}

}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	//--------------------------------------
	//SetParticleEnergy

	double energy;
	double weight;

	switch (xrType)
	{
	case MONO:
		//energy = 59.5; //original value [keV]
		//energy = 1.38E-3; // optical photon [keV]
		//energy = 88.03; // gamma [keV]
		energy = 59.5; // gamma [keV]
		//energy = 25; // gamma [keV]

		//Cd 88.03 [keV]
		//X-ray tube 35[keV]

		break;

	case SPECTER_continuous:
		do
		{
			energy = nMaxDataLines*G4UniformRand();
			weight = dMaxWeight*G4UniformRand();
		} while (weight > dWeight[(int)energy]);

		break;

	case SPECTER_discrete:
	{
		double rnd = G4UniformRand();
		for (int i = 0; i < Ev.size(); i++)
		{
			if (rnd < pv_edge[0])
			{
				energy = Ev[0] * 1E-3;
				//cout << "rnd < pv_edge[0]" << endl;
			}
			else if(rnd < pv_edge[i] && rnd > pv_edge[i-1])
			{
				energy = Ev[i] * 1E-3;
				//cout << "rnd < pv_edge[i] && rnd > pv_edge[i-1]" << endl;
			}			
		}


		break;
	}


	}


	particleGun->SetParticleEnergy(energy*keV);


	//-------------------------------------
	//Set particle direction

	const double pi = 3.1415926;

#ifdef DIRECT_INCIDENCE
	double phi = 0 * deg;
	double theta = 0 * deg;
	//cout << "inside DIRECT_INCIDENCE" << endl;
	//system("pause");
#endif //DIRECT_INCIDENCE	
	
#ifdef	SPHERE_4PI
	//uniform distribution
	double phi = 2 * pi*G4UniformRand();
	//double theta = pi/2*G4UniformRand();
	//double theta = acos((G4UniformRand() - 0.5) * 2);
	double cosTheta = (G4UniformRand() - 0.5) * 2;
#endif //SPHERE_4PI	


#ifdef TOP_HEMISPHERE
	double phi = 2 * pi*G4UniformRand();
	double cosTheta = G4UniformRand();
#endif //TOP_HEMISPHERE


#ifdef	ANGLE_Cd_COLL6mm
	double phi = 2 * pi*G4UniformRand();
	double theta_max = atan( (g()->h_c) / (2 * g()->l_x) );
	//cout << "l_x =" << g()->l_x << endl;
	//cout << "l_L =" << g()->l_L << endl;
	//cout << "h_c =" << g()->h_c << endl;
	//cout << "theta_max = " << theta_max << endl;
	double cosTheta = cos(theta_max) + (1 - cos(theta_max)) * G4UniformRand();
	
	//cout << "cosTheta = " << cosTheta << endl;
	//system("pause");
#endif //ANGLE_Cd_COLL6mm

	double sinTheta = sqrt(1 - cosTheta*cosTheta);
	particleGun->SetParticleMomentumDirection(G4ThreeVector(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta));
	//------------------------------------






	//-------------------------------------
	//set particle position
	int G_THGEM = 400;
	int avalanche_light_yield = 4; // ph per e
	int ph_per_hole = G_THGEM * avalanche_light_yield;

	double x, y, z;


#ifdef CENTRAL_INCIDENCE
	//x = 1.1 * mm * 1.5  ;
	//y = 1.1 * mm * sqrt(3) / 2 * 2;
	x = g()->x_source;
	y = g()->y_source;
	g()->z_source = 54.7;
	z = g()->z_source;
#endif //CENTRAL_INCIDENCE

#ifdef GEM_HOLE
	double radius = 0.250;
	
	x_shift = 0.9 * mm * (0.5);
	y_shift = 0;
	
	
	//if (particle_counter % ph_per_hole == 0)
	//{
	//	double step = 0.9;
	//	int rows = 111;
	//	int rows_half = rows / 2;
	//	int copy_number = G4UniformRand() * rows * rows;
	//	x_shift = (copy_number % rows - rows_half) * step;
	//	y_shift = (copy_number / rows - rows_half) * step*sqrt(3) / 2;
	//	if ((copy_number / rows - rows_half) % 2 == 0)
	//	{
	//		x_shift -= step / 2.0;
	//	}
	//	//x_shift = (G4UniformRand() - 0.5) * 100;
	//}

	cout << "particle_counter = " << particle_counter << "\t x_shift = " << x_shift << "\t y_shift = " << y_shift << endl;
	particle_counter++;

	do
	{
		x = 2 * radius*(G4UniformRand() - 0.5)*mm + x_shift;
		y = 2 * radius*(G4UniformRand() - 0.5)*mm + y_shift; // +1.1 * mm * sqrt(3) / 2 * (-1);
		z = 77.7;
	} while (pow(x - x_shift, 2.0) + pow(y - y_shift, 2.0) > pow(radius, 2.0)*mm2);
#endif //GEM_HOLE


#ifdef CIRCLE_EL_GAP
	//circle
	const double angle = G4UniformRand() * 3.1415926 * 2;
	const double radius = 2.35/2.0;
	while (true)
	{
		double x_tmp = (G4UniformRand() - 0.5) * 2 * radius;
		double y_tmp = (G4UniformRand() - 0.5) * 2 * radius;
		if (x_tmp*x_tmp + y_tmp*y_tmp < radius*radius)
		{
			x = x_tmp;
			y = y_tmp;
			z = 54.7 + (72.7 - 54.7)*G4UniformRand();
			break;
		}
	}
#endif // CIRCLE


#ifdef EL_GAP
	x = g()->x_source;
	y = g()->y_source;
	z = 54.7 + (72.7 - 54.7)*G4UniformRand();
#endif //EL_GAP


#ifdef	CIRCLE_Cd
	//circle
	/*const double angle = G4UniformRand() * 3.1415926 * 2;
	const double radius = g()->radius_source_gamma;
	while (true)
	{
		double x_tmp = (G4UniformRand() - 0.5) * 2 * radius;
		double y_tmp = (G4UniformRand() - 0.5) * 2 * radius;
		if (x_tmp*x_tmp + y_tmp*y_tmp < radius*radius)
		{
			x = x_tmp;
			y = y_tmp;			
			break;
		}
	}*/
	x = 0;
	y = 0;
	z = g()->z_source_gamma;
#endif //CIRCLE_Cd

	particleGun->SetParticlePosition(G4ThreeVector(x, y, z));
	//------------------------------------


	particleGun->GeneratePrimaryVertex(anEvent);

	G4int  n_event = anEvent->GetEventID();
	if ((n_event % 1000) == 0)
	{
		//cout << "New event started! " << anEvent->GetEventID() << " Particle energy: " << energy << " keV. " << endl;
	}

}


