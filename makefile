# Use g++ compiler
CC=g++

# Compiler flags: -Wall (show all warnings) -O3 use optimizations
CFLAGS=-std=c++11 -Wall -O3 -I/local/lib

# Linker flags
LFLAGS=-L/local/lib -lboost_program_options -lboost_system -lboost_filesystem

abp: ABP.hpp Vector.hpp main.cpp
	$(CC) $(CFLAGS) main.cpp $(LFLAGS) -o abp
