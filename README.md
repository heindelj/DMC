# DMC
An implementation of Diffusion Monte Carlo (DMC). Can do both discrete- and continuous-weighted DMC simulations.

Comes only with a harmonic oscillator potential by default, but custom potentials can be easily added by creating a child of the Potential class and overriding the virtual function, calcEnergy(), belonging to the Potential class.
See how this is done for the HO potential for an example.

DEPENDENCIES:
Currently depends on the glm math library. This is a header-only library which can be found on github: https://github.com/g-truc/glm

Usage:

For convenience, the glm library is contained in the include folder in this repo. Make sure to point your compiler to this folder as part of the include path.

All possible options which can be given are in input_file.txt as an example. The file should provide an option (defined in readInputFile.h) separated by a ":" from the desired value. DMCMODE can only take values "CONTINUOUS" or "DISCRETE". All input is case-insensitive.

Run the code by compiling and using, for example,

> ./DMC.exe input_file.txt

All relevant information: walkers, corresponding weights (for continuous DMC), and running average of reference energy will be output to the files specified in the input file with intervals corresponding to the WALKERSTRIDE and ENERGYSTRIDE.

TODO:
Remove dependency on glm.
Make DMC functional for 1D and 2D input. (Currently only works for 3D systems).
