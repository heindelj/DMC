#pragma once

#include <map>
#include <string>

// All masses come from NIST at the following domain:
// https://physics.nist.gov/cgi-bin/Compositions/stand_alone.pl
// All masses are those of individual isotopes, and not the weighted
// average normally found on periodic tables.
struct atomicMasses {
	std::map<std::string, double> masses =
	{
		{"X", 1.000000000},
		{"H", 1.00782503223},
		{"D", 2.01410177812},
		{"T", 3.0160492779},
		{"He", 4.00260325413},
		{"Li", 7.0160034366},
		{"Be",  9.012183065},
		{"B", 11.00930536},
		{"C", 12.0000000},
		{"N", 14.00307400443},
		{"O", 15.99491461957},
		{"F", 18.99840316273},
		{"Ne", 19.9924401762},
		{"Cl", 34.968852682},
		{"Br", 78.9183376},
		{"I", 126.9044719}
	};
};