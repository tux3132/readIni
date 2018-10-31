testReadIni: readIni.o main.o
	g++ -Wall -fno-exceptions -g -O0 -I. -o testReadIni readIni.o main.o
readIni.o: readIni.c
	g++ -Wall -fno-exceptions -g -O0 -I. -c readIni.c -o readIni.o
main.o: main.cpp readIni.h
	g++ -Wall -fno-exceptions -g -O0 -I. -o main.o -c main.cpp
