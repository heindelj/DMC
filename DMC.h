#pragma once

#include "Molecule.h"
#include "Potential.h"
#include "readInputFile.h"

#include <glm/glm.hpp>

#include <vector>      // walker container
#include <string>	   // logging
#include <random>      // sampling functions
#include <time.h>      // seed for random numbers
#include <iostream>	   // logging
#include <memory>      // unique_ptr
#include <algorithm>   // for_each
#include <execution>   // par_unseq
#include <numeric>     // accumulate
#include <iterator>    // distance
#include <math.h>      // sqrt

class DMC
{
public:
	DMC(Potential* potential, inputFile input);

	//getters
	double getRefEnergy();

protected:
	std::vector<Molecule> m_Walkers; // contains all currently alive walkers
	std::vector<double> m_Weights;   // contains weights associated with all walkers
	std::vector<double> m_Sigmas;    // contains the std. dev. for each mass (compute only once)

	void initDMC(std::string initFile, std::vector<Molecule>& walkers);

	inputFile m_Input;

	template <typename T>
	void moveWalkers(T& walkers);
	void moveWalker(Molecule& walker);

	template <typename T>
	void calcWalkerEnergies(T& walkers);

	template <typename T>
	double averagePotentialEnergy(T& walkers);

	double m_RefEnergy;
	double m_Alpha = 10.0;

	// Input parameters
	Potential* m_Potential;
	int m_NumDesiredWalkers; //number of walkers set
	double m_dt; //time step
	int m_MaxSteps;

	// RNG stuff
	typedef std::mt19937_64 RNG;
	RNG m_RNG;
	std::uniform_real_distribution<double> m_Uniform;
	std::normal_distribution<double> m_Normal;
	double sampleNormal(double mu, double sigma);
	double sampleUniform();

	// For Logging
	std::vector<double> m_RunningAverageRefEnergy;
	std::vector<double> m_RunningAveragePotential;

private:
	virtual void calcRefEnergy();
	friend class LogDMCOutput;
};

/////////////////////////////////////////////////////////////////////
///////////////////Discrete Weighted DMC/////////////////////////////
/////////////////////////////////////////////////////////////////////

class DMC_Discrete : public DMC
{
public:
	DMC_Discrete(Potential* potential, inputFile input);

	void runDMC();

private:
	void calcRefEnergy();
	void creationProcess();
	void birthWalker(Molecule walker);
	void killWalker(int index);
};

/////////////////////////////////////////////////////////////////////
///////////////////Continuous Weighted DMC///////////////////////////
/////////////////////////////////////////////////////////////////////

class DMC_Continuous : public DMC
{
public:
	DMC_Continuous(Potential* potential, inputFile input);
	void runDMC();

private:
	void calcRefEnergy();
	void updateWeights();
	void redistributeWeight(int index);
	double sumWeights();
	double calcAveragePotential();
};