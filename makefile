CXX = g++ 
OBJECTS= chessboard.o main.o
BINARY=program

all: CPPFLAGS = -std=c++11
all: CFLAGS = 
all: program

optimal: CFLAGS=-Wdivision-by-zero -Ofast -march=native -flto -ffast-math
optimal: CPPFLAGS=-std=c++11 -Wdivision-by-zero -Ofast -march=native -flto -ffast-math
optimal: program


program: $(OBJECTS)
	$(CXX) $(CPPFLAGS) -o $(BINARY) $(OBJECTS)

chessboard.o: chessboard.cpp chessboard.h
	$(CXX) $(CPPFLAGS) -c chessboard.cpp -o chessboard.o

main.o: main.cpp 
	$(CXX) $(CPPFLAGS) -c main.cpp -o main.o

clean:
	rm -f *.o $(BINARY)

