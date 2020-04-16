#include "Molecule.h"
#include <sstream>
#include <fstream>
#include <filesystem> // exists
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


//constructor which reads xyz formatted file
Molecule::Molecule(std::string iFile)
{
	readXYZ(iFile);
	atomicMasses massMap;
	setMassesFromInput(massMap);
	m_Energy = 0.0;
}

Molecule::~Molecule()
{
	m_Coordinates.erase(m_Coordinates.begin(), m_Coordinates.end());
	m_AtomLabels.erase(m_AtomLabels.begin(), m_AtomLabels.end());
}

void Molecule::readXYZ(const std::string& file)
{
	if (!std::filesystem::exists(file))
	{
		throw "Could not find geometry file (.xyz)!";
	}
		
	//open the file
	std::ifstream infile(file);

	std::string line;
	//read the number of atoms and comment line, then parse by tokens
	std::getline(infile, line);
	m_NumAtoms = std::stoi(line);
	//pre-allocate the space for vectors
	m_AtomLabels.reserve(m_NumAtoms);
	m_Coordinates.reserve(3 * m_NumAtoms);

	std::getline(infile, line);
	m_Comment = line;

	//read all of the coordinates and atom labels
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string atom;
		float x, y, z;
		if (!(iss >> atom >> x >> y >> z))
			break; //incorrect format for xyz file. Throw some error here.
		m_AtomLabels.push_back(atom);
		m_Coordinates.push_back(glm::vec3(x, y, z));
	}
}

void Molecule::setMassesFromInput(atomicMasses massMap)
{
	for (std::string atomLabel : m_AtomLabels)
	{
		atomLabel[0] = toupper(atomLabel[0]);
		m_Mass.push_back(massMap.masses[atomLabel]);
	}
}

//getters and setters

unsigned int Molecule::getNumAtoms() const
{
	return m_NumAtoms;
}

double Molecule::getEnergy() const
{
	return m_Energy;
}

std::vector<glm::vec3>* Molecule::getCoordinatesPointer()
{
	return &m_Coordinates;
}

std::vector<double>* Molecule::getMassPointer()
{
	return &m_Mass;
}

std::vector<glm::vec3> Molecule::getCoordinates() const
{
	return m_Coordinates;
}

std::vector<std::string> Molecule::getAtomLabels() const
{
	return m_AtomLabels;
}

std::vector<double> Molecule::getMass() const
{
	return m_Mass;
}

void Molecule::setEnergy(double energy)
{
	m_Energy = energy;
}