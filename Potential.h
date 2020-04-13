#pragma once

#include "Molecule.h"
#include <memory>

class Potential
{
	// This class is basically just for documentation.
	//every actual potential should inherit from this and have it's own implementation.
public:
	virtual void calcEnergy(Molecule& molecule);
};


class HarmonicOscillator : public Potential
{
	double m_Omega;

public:
	HarmonicOscillator(double omega);
	void calcEnergy(Molecule& molecule) override;
};

