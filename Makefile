CC=g++
CFLAGS=-Wall -Wextra -fno-exceptions -g -pedantic -O0 -I.
EXEC=testReadIni

all:	$(EXEC)

testReadIni: main.o readIni.o
	$(CC) -o $@ readIni.o main.o $(LDFLAGS)

readIni.o: readIni.cpp
	$(CC) -o $@ -c readIni.cpp $(CFLAGS)

main.o: main.cpp readIni.h
	$(CC) -o $@ -c main.cpp $(CFLAGS)

clean:
	rm -f *.o core
	
mrproper: clean
	rm -f $(EXEC)
