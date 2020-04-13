#include "Molecule.h"
#include "DMC.h"
#include "Potential.h"

int main(int argc, char* argv[])
{
	// provide parameters to potential energy surface 
	// (user will need to create their own Potential class
	std::unique_ptr<Potential> HO = std::make_unique<HarmonicOscillator>(1.0);
	// read input file as command line argument
	if (argc < 2)
	{
		std::cout << "Did not receive an input file! Usage is as follows:" << std::endl;
		std::cout << argv[0] << "   [input file]" << std::endl;
		return 1;
	}
	inputFile input(argv[1]);
	// based on mode given in input, run either discrete or continuous DMC
	std::string dmcMode = input.m_InputArgs["DMCMODE"];
	std::transform(dmcMode.begin(), dmcMode.end(), dmcMode.begin(), toupper);
	if (dmcMode == "CONTINUOUS")
	{
		DMC_Continuous runDMC = DMC_Continuous(HO.get(), input);
		runDMC.runDMC();
	}
	else if (dmcMode == "DISCRETE")
	{
		DMC_Discrete runDMC = DMC_Discrete(HO.get(), input);
		runDMC.runDMC();
	}
	else
	{
		std::cout << "Invalid DMC Mode. Exiting.";
	}
}
