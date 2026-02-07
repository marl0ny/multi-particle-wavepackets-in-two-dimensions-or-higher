# Wave function visualization of two 2D particles (WIP)

Interactive simulation of two interacting quantum particles confined to a 2D square domain. This uses an [explicit, 2nd-order centred finite difference in time method](https://doi.org/10.1063/1.168415) to numerically solve the time-dependent Schrödinger equation, in which the real and imaginary parts of the wave function are separately updated at staggered time steps in a "leapfrog-like" manner.

### References

 - Visscher, P. (1991). A fast explicit algorithm for the time‐dependent Schrödinger equation. <em>Computers in Physics, 5</em>, 596-598. [https://doi.org/10.1063/1.168415](https://doi.org/10.1063/1.168415)