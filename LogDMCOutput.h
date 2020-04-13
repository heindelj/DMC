#pragma once

#include "DMC.h"
#include "readInputFile.h"
#include <fstream>

class LogDMCOutput
{
	unsigned int m_WalkerStride = 1000; // number of steps between logging all walkers and energies
	unsigned int m_EnergyStride = 10; // number of steps between printing running average energy values
	unsigned int m_RunningAverageDelay = 1000; // steps before beginning running average
	std::string m_WalkerFile = "dmc_walkers.out";
	std::string m_EnergyFile = "dmc_running_average.out";
	unsigned int m_NumCalls = 0; // for tracking when to print things

public:
	LogDMCOutput(inputFile input);

	~LogDMCOutput();

	void logOutput(const DMC& dmc);

private:
	void logWalkers(const DMC& walkers);
	void logEnergies(const double& refEnergy);
	friend class DMC;
	friend class DMC_Discrete;
	friend class DMC_Continuous;
};

