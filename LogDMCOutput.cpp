#include "LogDMCOutput.h"

LogDMCOutput::LogDMCOutput(inputFile input)
{
	m_EnergyFile = input.m_InputArgs["OUTPUTENERGIES"];
	m_WalkerFile = input.m_InputArgs["OUTPUTWALKERS"];
	m_EnergyStride = stoi(input.m_InputArgs["ENERGYSTRIDE"]);
	m_WalkerStride = stoi(input.m_InputArgs["WALKERSTRIDE"]);
	m_RunningAverageDelay = stoi(input.m_InputArgs["RUNNINGAVERAGEDELAY"]);
	m_NumCalls = 0;
}

LogDMCOutput::~LogDMCOutput() {}

void LogDMCOutput::logOutput(const DMC& dmc)
{
	if ((m_NumCalls % m_WalkerStride) == 0)
		logWalkers(dmc);
	if ((m_NumCalls % m_EnergyStride) == 0 && m_NumCalls >= m_RunningAverageDelay)
	{
		double averageRefEnergy = 0.0;
		for (int i = 0; i < dmc.m_RunningAverageRefEnergy.size(); i++)
		{
			averageRefEnergy += dmc.m_RunningAverageRefEnergy[i];
		}
		logEnergies(averageRefEnergy / (double)dmc.m_RunningAverageRefEnergy.size());
	}
	m_NumCalls += 1;
}

// print out all of the walkers
void LogDMCOutput::logWalkers(const DMC& dmc)
{
	std::ofstream walkerFile;
	walkerFile.open(m_WalkerFile, std::ofstream::out | std::ofstream::app);
	walkerFile << "--------------- STEP " << m_NumCalls << " ---------------" << std::endl;
	for (int i = 0; i < dmc.m_Walkers.size(); i++)
	{
		walkerFile << dmc.m_Walkers[i].getNumAtoms() << std::endl;
		if (dmc.m_Weights.empty())
			walkerFile << dmc.m_Walkers[i].getEnergy() << std::endl;
		else
			walkerFile << dmc.m_Walkers[i].getEnergy() << "   " << dmc.m_Weights[i] << std::endl;
		const std::vector<glm::vec3> coords = dmc.m_Walkers[i].getCoordinates();
		const std::vector<std::string> labels = dmc.m_Walkers[i].getAtomLabels();
		for (int j = 0; j < coords.size(); j++)
		{
			walkerFile << labels[j] << " " << coords[j][0] << " " <<
			coords[j][1] << " " << coords[j][2] << std::endl;
		}
	}
	walkerFile.close();
}

// print out the running average reference energy
void LogDMCOutput::logEnergies(const double& refEnergy)
{
	std::ofstream energyFile;
	energyFile.open(m_EnergyFile, std::ofstream::out | std::ofstream::app);
	energyFile << m_NumCalls << "       " << refEnergy << std::endl;
	energyFile.close();
}