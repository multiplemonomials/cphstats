install:
	g++ -c -o main.o main.cpp
	g++ -c -o cpin.o cpin.cpp
	gfortran -c -o parse_cpin.o parse_cpin.F90
	g++ -o cphutil main.o cpin.o parse_cpin.o -lgfortran
