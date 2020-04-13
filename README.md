# DMC
An implementation of Diffusion Monte Carlo (DMC). Can do both discrete- and continuous-weighted DMC simulations.

Comes only with a harmonic oscillator potential by default, but custom potentials can be easily added by creating a child of the Potential class and overriding the virtual function, calcEnergy(), belonging to the Potential class.
See how this is done for the HO potential for an example.

DEPENDENCIES:
Currently depends on the glm math library. This is a header-only library which can be found on github here: https://github.com/g-truc/glm

TODO:
Remove dependency on glm.
Make DMC functional for 1D and 2D input. (Currently only works for 3D systems).
