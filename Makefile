CC = g++

LIBS =

.PHONY: clean

all: ihex
	g++ test.cpp *.o -o test 

ihex:
	g++ -c ihex2bin.cxx	-fpack-struct

clean:
	rm -rf *.o
	rm -rf test
