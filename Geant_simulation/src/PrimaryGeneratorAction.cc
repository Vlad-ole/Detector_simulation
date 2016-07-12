#include <iostream>
using namespace std;

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "Randomize.hh"

#include <G4SystemOfUnits.hh> // this has appeared in GEANT4_10

//#define DIRECT_INCIDENCE
#define CENTRAL_INCIDENCE

void PrimaryGeneratorAction::CommonPart()
{
	
	
	G4int n_particle = 1;
	particleGun  = new G4ParticleGun(n_particle);
	
	// default particle kinematic
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;
	G4ParticleDefinition* particle = particleTable->FindParticle(particleName= /*"gamma"*/ "opticalphoton");

	particleGun->SetParticleDefinition(particle);
	
}

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
	CommonPart();
	xrType = MONO;
}

PrimaryGeneratorAction::PrimaryGeneratorAction(const char* fname)
{
	CommonPart();
	xrType = SPECTER;

	char   buffer[256];
	char   seps[] = " \t;";	  //separatoren

	nMaxDataLines = 0;
	dMaxWeight  = 0; //!!! // dMaxWeight  = 1;


	cout << "Read Specter: " << fname << endl;
	FILE* in =  fopen(fname,"r");
	if(in == NULL)
	{
		cout << "Specter of X-Tube not loaded!!! " << fname << endl;
		return;
	}

	while (!feof(in) && (nMaxDataLines < MAX_SPECTER_DATALINES))
	{
		fgets(buffer,sizeof(buffer)-1,in);
		cout << "Read of " << nMaxDataLines << " line: " << buffer <<endl;
		
		if (strlen(buffer)>2)
		{
			if (!strstr(buffer,"//"))
			{
				dEnergy[nMaxDataLines] = atoi (strtok(buffer,seps));
				dWeight[nMaxDataLines] = atof (strtok(NULL,  seps));
				
				if( dWeight[nMaxDataLines] > dMaxWeight )
					dMaxWeight = dWeight[nMaxDataLines];
				
				nMaxDataLines++;
			}
		}
	}
	cout << "Specter " << fname << "loaded! There are " << nMaxDataLines << " datalines." <<endl;

	fclose(in);

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
		
	switch(xrType)
	{
	case MONO:
		//energy = 59.5; //original value [keV]
		energy = 2.8E-3; // optical photon [keV]

		break;

	case SPECTER:
		do
		{
			energy = nMaxDataLines*G4UniformRand();
			weight = dMaxWeight*G4UniformRand();
		}while(weight > dWeight[(int)energy]);

		break;
	}

	particleGun->SetParticleEnergy(energy*keV);


	//-------------------------------------
	//Set particle direction


	const double pi = 3.1416;

#ifdef DIRECT_INCIDENCE
	double phi = 0 * deg;
	double theta = 10 * deg;
	//cout << "inside DIRECT_INCIDENCE" << endl;
	//system("pause");
#else 
	//uniform distribution
	double phi = 2*pi*G4UniformRand();
	//double theta = pi/2*G4UniformRand();
	double theta = acos( G4UniformRand() );
#endif //DIRECT_INCIDENCE		

	particleGun->SetParticleMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)));
	//------------------------------------
	
	
	//-------------------------------------
	//set particle position

	double x, y, z;
#ifdef CENTRAL_INCIDENCE
	//x = 1.1 * mm * 1.5  ;
	//y = 1.1 * mm * sqrt(3) / 2 * 2;
	x = 0;
	y = 0;
	//cout << "inside CENTRAL_INCIDENCE" << endl;
	//system("pause");
#else
	double radius = 1; //0.250;
	//do
	//{
	x = 2 * radius*(G4UniformRand() - 0.5)*mm + 0*mm;// +1.1 * mm * (-1);
	y = 2 * radius*(G4UniformRand() - 0.5)*mm; // +1.1 * mm * sqrt(3) / 2 * (-1);
	z = 0;
	//} while (x*x + y*y > pow(radius, 2.0)*mm2);
#endif
	particleGun->SetParticlePosition(G4ThreeVector(x, y, z));
	//------------------------------------
	
	
	particleGun->GeneratePrimaryVertex(anEvent);
	
	G4int  n_event = anEvent->GetEventID();
	if((n_event%1000)==0)
	{
		cout << "New event started! " << anEvent->GetEventID() << " Particle energy: "  << energy << " keV. "<< endl; 
	} 

}


