# configured with [[ ./configure --prefix=/usr/local gnu ]]

# Compilers
CXX = g++
F90 = gfortran
LD = g++

# Flags
F90FLAGS = -Wall -O3 -mtune=native
CXXFLAGS = -Wall -O3 -mtune=native
LDFLAGS = -lgfortran -lz -O3 -mtune=native

# Name of the program
PROGNAME = cphstats
PREFIX = /usr/local
