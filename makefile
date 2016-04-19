CXX = g++ 
OBJECTS= bin/chessboard.o bin/main.o
BINARY= ./bin/program 

all: CPPFLAGS = -std=c++11
all: CFLAGS = 
all: program

optimal: CFLAGS=-Wdivision-by-zero -Ofast -march=native -flto -ffast-math
optimal: CPPFLAGS=-std=c++11 -Wdivision-by-zero -Ofast -march=native -flto -ffast-math
optimal: program

program: $(OBJECTS)
	$(CXX) $(CPPFLAGS) -o $(BINARY) $(OBJECTS)

bin/chessboard.o: chessboard.cpp chessboard.h
	$(CXX) $(CPPFLAGS) -c chessboard.cpp -o bin/chessboard.o

bin/main.o: main.cpp 
	$(CXX) $(CPPFLAGS) -c main.cpp -o bin/main.o

clean:
	rm -f bin/*.o $(BINARY)

