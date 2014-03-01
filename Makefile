# Makefile for LinearSim
# By Thomas Steinke and Tyler Mao

CPP = gcc
CPPFLAGS = -g -O3

LinearSim: Cell LinearSim.c
	$(CPP) $(CPPFLAGS) LinearSim.c -o LinearSim

Cell: Cell.c
	$(CPP) $(CPPFLAGS) Cell.c -o Cell

clear:
	rm Cell LinearSim
