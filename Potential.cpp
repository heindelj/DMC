#include "Potential.h"

void Potential::calcEnergy(Molecule& molecule)
{
}


HarmonicOscillator::HarmonicOscillator(double omega)
	: m_Omega(omega)
{
}

void HarmonicOscillator::calcEnergy(Molecule& molecule)
{
	double energy = 0.0;
	std::vector<glm::vec3>* coords = molecule.getCoordinatesPointer();
	std::vector<double>* mass = molecule.getMassPointer();
	for (int i = 0; i < (*coords).size(); i++)
		energy += 0.5 * (*mass)[i] * m_Omega * m_Omega * glm::length((*coords)[i]) * glm::length((*coords)[i]);
	molecule.setEnergy(energy);
}

