#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm> // transform

class inputFile
{

	std::string m_iFile;

public:
	std::map<std::string, std::string> m_InputArgs = {
		{"DMCMODE", "CONTINUOUS"},
		{"NUMWALKERS", "2000"},
		{"DT", "0.005"},
		{"MAXSTEPS", "100000"},
		{"ENERGYSTRIDE", "10"},
		{"WALKERSTRIDE", "1000"},
		{"RUNNINGAVERAGEDELAY", "1000"},
		{"INPUTGEOMETRY", "a.xyz"},
		{"OUTPUTWALKERS", "dmc_walkers.out"},
		{"OUTPUTENERGIES", "dmc_energies.out"}
	};
	inputFile(std::string iFile);
	inputFile();

private:
	void setInput();
};

