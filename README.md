# Description
Simulation of a tracer in an active chiral bath

# Citation
If you use our code for your own research, we ask you to cite the following paper:
R. Goerlich, A.P. Antonov, K.S. Olsen, L. Caprini, C. Scholz, H. Löwen, Y. Roichmann, arXiv:2605.25136 (2026)

# Dependencies
Our software requires boost, which can be downloaded from the following source:
https://www.boost.org/

Random numbers were generated using the Mersenne Twister algorithm implemented in MersenneTwister.h (Matsumoto & Nishimura, 1998).

# How to use
To compile the code, use the provided makefile:

make abp

To see available parameters, run ./abp --help

In the outputs, the unit of length is 1 cm, and the unit of time is 0.1 s
