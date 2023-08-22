all: vgslz4

vgslz4: vgslz4.cpp ../../src/lz4.c ../../include/lz4.h
	gcc -I../../include ../../src/lz4.c -c -o lz4.o
	g++ -std=c++17 -I../../include vgslz4.cpp lz4.o -o vgslz4
