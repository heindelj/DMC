#include "DMC.h"
#include "LogDMCOutput.h"

DMC::DMC(Potential* potential, inputFile input)
	: m_Potential(potential)
{
	m_RefEnergy = 0.0;
	m_Input = input;

	// input file variables
	m_NumDesiredWalkers = stoi(input.m_InputArgs["NUMWALKERS"]);
	m_MaxSteps = stoi(input.m_InputArgs["MAXSTEPS"]);
	m_dt = stod(input.m_InputArgs["DT"]);
	m_Alpha = stod(input.m_InputArgs["ALPHA"]);

	// random number stuff
	std::random_device rd;
	m_RNG = RNG(rd());
	m_RNG.seed(::time(NULL));
}

void DMC::initDMC(std::string initFile, std::vector<Molecule>& walkers)
{
	for (int i = 0; i < m_NumDesiredWalkers; i++)
		walkers.push_back(Molecule(initFile));
}

void DMC::calcRefEnergy() {}

double DMC::sampleNormal(double mu, double sigma)
{
	m_Normal = std::normal_distribution<double>(mu, sigma);
	return m_Normal(m_RNG);
}
double DMC::sampleUniform()
{
	// random number generator with seed
	m_Uniform = std::uniform_real_distribution<double>(0.0, 1.0);
	return m_Uniform(m_RNG);
}

double DMC::getRefEnergy()
{
	return m_RefEnergy;
}

template <typename T>
double DMC::averagePotentialEnergy(T& walkers)
{
	double energy = 0.0;
	for (int i = 0; i < walkers.size(); i++)
		energy += walkers[i].getEnergy();
	return energy / (double)walkers.size();
}

template <typename T>
void DMC::calcWalkerEnergies(T& walkers)
{
	// Updates energies of all walkers based on their current configurations
	std::for_each(std::execution::par_unseq,
		walkers.begin(), walkers.end(), [this](Molecule& molecule)
		{
			m_Potential->calcEnergy(molecule);
		});
}

// Make random movements of atoms dependent on their masses
void DMC::moveWalker(Molecule& walker)
{
	// most time spent here. Optimize this further.
	std::vector<double>* mass = walker.getMassPointer();
	std::vector<glm::vec3>* coords = walker.getCoordinatesPointer();
	for (int i = 0; i < (*coords).size(); i++)
	{
		double sigma = glm::sqrt(m_dt / (*mass)[i]);
		(*coords)[i] += glm::vec3(sampleNormal(0.0, sigma),
			sampleNormal(0.0, sigma),
			sampleNormal(0.0, sigma));
	}
}

// most likely T is either std::vector or std::array
template <typename T>
void DMC::moveWalkers(T& walkers)
{
	std::for_each(std::execution::par_unseq,
		walkers.begin(), walkers.end(), [this](Molecule& molecule)
		{
			moveWalker(molecule);
		});
}

/////////////////////////////////////////////////////////////////////
///////////////////Discrete Weighted DMC/////////////////////////////
/////////////////////////////////////////////////////////////////////

DMC_Discrete::DMC_Discrete(Potential* potential, inputFile input)
	: DMC(potential, input)
{
	// create walkers
	m_Walkers.reserve(m_NumDesiredWalkers);
	initDMC(input.m_InputArgs["INPUTGEOMETRY"], m_Walkers);
}

void DMC_Discrete::runDMC()
{
	LogDMCOutput logger = LogDMCOutput(m_Input);
	for (int i = 0; i < m_MaxSteps; i++)
	{
		moveWalkers(m_Walkers);
		calcWalkerEnergies(m_Walkers);
		creationProcess();
		if (i >= logger.m_RunningAverageDelay)
		{
			m_RunningAverageRefEnergy.push_back(m_RefEnergy);
			m_RunningAveragePotential.push_back(averagePotentialEnergy(m_Walkers));
		}
		logger.logOutput(*this);
	}
}


// Calculate reference from all walkers potential energy, size of population,
// and desired population size.
void DMC_Discrete::calcRefEnergy()
{
	m_RefEnergy = 0.0;
	// make this it's own function and parallelize
	for (int i = 0; i < m_Walkers.size(); i++)
		m_RefEnergy += m_Walkers[i].getEnergy();
	m_RefEnergy /= (double)m_Walkers.size();
	m_RefEnergy -= m_Alpha * ((double)m_Walkers.size() - (double)m_NumDesiredWalkers) / (double)m_NumDesiredWalkers;
}

// Evaluate creation and annihilation of walkers
void DMC_Discrete::creationProcess()
{
	calcRefEnergy();
	// TODO: parallelize this part
	std::vector<int> indicesToDelete;
	indicesToDelete.reserve(m_Walkers.size() / 2);
	std::vector<int> indicesToCopy;
	indicesToCopy.reserve(m_Walkers.size() / 2);
	// do the loop in reverse so that indices are reverse sorted
	for (std::vector<int>::size_type i = m_Walkers.size() - 1; i != (std::vector<int>::size_type) - 1; i--)
	{
		double Pc = glm::exp((m_RefEnergy - m_Walkers[i].getEnergy()) * m_dt);

		// death process
		if (Pc < 1.0)
		{
			double Rn = sampleUniform();
			if (Pc < Rn)
				indicesToDelete.push_back(i);
		}
		// birth process
		else
		{
			double Rn = sampleUniform();
			// there is already one copy, and int(Pc) is the 
			// total number of copies we should have
			while (Pc > 2.0)
			{
				indicesToCopy.push_back(i);
				Pc -= 1.0;
			}
			if ((Pc - 1.0) > Rn)
				indicesToCopy.push_back(i);
		}

	}
	// first add created walkers
	for (int index : indicesToCopy)
		birthWalker(m_Walkers[index]);
	// now delete the destroyed walkers
	// the indices ar reverse sorted, so there should be no problem
	// with deleting them from a dynamic vector
	for (int index : indicesToDelete)
		killWalker(index);
}

// Death of Walker
void DMC_Discrete::killWalker(int index)
{
	m_Walkers.erase(m_Walkers.begin() + index);
}

// Birth of Walker
void DMC_Discrete::birthWalker(Molecule walker)
{
	m_Walkers.push_back(walker);
}

/////////////////////////////////////////////////////////////////////
///////////////////Continuous Weighted DMC///////////////////////////
/////////////////////////////////////////////////////////////////////

DMC_Continuous::DMC_Continuous(Potential* potential, inputFile input)
	: DMC(potential, input)
{
	// create walkers
	m_Walkers.reserve(m_NumDesiredWalkers);
	initDMC(input.m_InputArgs["INPUTGEOMETRY"], m_Walkers);
	// pre-initialize all walkers with weight of 1
	m_Weights.resize(m_NumDesiredWalkers, 1.0); // note resize not reserve
	assert(m_Walkers.size() == m_Weights.size());
}

void DMC_Continuous::runDMC()
{
	LogDMCOutput logger = LogDMCOutput(m_Input);
	for (int i = 0; i < m_MaxSteps; i++)
	{
		moveWalkers(m_Walkers);
		calcWalkerEnergies(m_Walkers);
		updateWeights();
		if (i >= logger.m_RunningAverageDelay)
		{
			m_RunningAverageRefEnergy.push_back(m_RefEnergy);
			m_RunningAveragePotential.push_back(calcAveragePotential());
		}
		logger.logOutput(*this);
	}
}


void DMC_Continuous::updateWeights()
{
	calcRefEnergy();
	// TODO: parallelize this
	for (int i = 0; i < m_Walkers.size(); i++)
	{
		m_Weights[i] *= glm::exp((m_RefEnergy - m_Walkers[i].getEnergy()) * m_dt);
		if (m_Weights[i] < (1.0 / m_NumDesiredWalkers))
			redistributeWeight(i);
	}
}

// finds largest weight walker and cuts its weight in half
// while copying the walker to the index provided
void DMC_Continuous::redistributeWeight(int index)
{
	int maxIndex = std::distance(m_Weights.begin(), std::max_element(m_Weights.begin(), m_Weights.end()));
	m_Weights[maxIndex] /= 2.0;
	m_Weights[index] = m_Weights[maxIndex];
	m_Walkers[index] = m_Walkers[maxIndex];
}

void DMC_Continuous::calcRefEnergy()
{
	m_RefEnergy = calcAveragePotential();
	m_RefEnergy -= m_Alpha * ((double)sumWeights() - (double)m_NumDesiredWalkers) / (double)m_NumDesiredWalkers;
}

double DMC_Continuous::calcAveragePotential()
{
	double averagePotential = 0.0;
	for (int i = 0; i < m_Walkers.size(); i++)
		averagePotential += m_Weights[i] * m_Walkers[i].getEnergy(); //careful of types here if weights are ints

	return averagePotential / sumWeights();
}

double DMC_Continuous::sumWeights()
{
	return std::accumulate(m_Weights.begin(), m_Weights.end(), 0.0);
}