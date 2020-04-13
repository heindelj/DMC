#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "atomicMasses.h"


class Molecule
{
	std::vector<glm::vec3> m_Coordinates;
	std::vector<std::string> m_AtomLabels;
	std::vector<double> m_Mass;
	double m_Energy = 0.0;
	unsigned int m_NumAtoms;
	std::string m_Comment; // probably throw this away.

public:
	// constructor which reads xyz formatted file
	Molecule(std::string iFile);
	~Molecule();

	//getters and setters
	std::vector<glm::vec3>* getCoordinatesPointer();
	std::vector<double>* getMassPointer();
	std::vector<glm::vec3> getCoordinates() const;
	std::vector<double> getMass() const;
	std::vector<std::string> getAtomLabels() const;
	unsigned int getNumAtoms() const;
	double getEnergy() const;

	void setEnergy(double energy);

private:
	void readXYZ(std::string file);
	void setMassesFromInput(atomicMasses massMap);
};

