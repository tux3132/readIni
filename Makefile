CC=g++
CFLAGS=-Wall -Wextra -fno-exceptions -g -pedantic -O0 -I.
EXEC=testReadIni

all:	$(EXEC)

testReadIni: main.o readIni.o myFuncts.o
	$(CC) -o $@ readIni.o myFuncts.o main.o $(LDFLAGS)

readIni.o: readIni.cpp
	$(CC) -o $@ -c readIni.cpp $(CFLAGS)

myFuncts.o: myFuncts.cpp
	$(CC) -o $@ -c myFuncts.cpp $(CFLAGS)

main.o: main.cpp readIni.h myFuncts.h
	$(CC) -o $@ -c main.cpp $(CFLAGS)

clean:
	rm -f *.o core
	
mrproper: clean
	rm -f $(EXEC)
