CXX=g++
#CXXFLAGS=-Wall -pedantic-errors -g -O3 -march=native -mtune=native
CXXFLAGS=-Wall -pedantic-errors -pg
#CXXFLAGS=-Wall -pedantic-errors -g
SOURCES=src/attack.cpp src/bench.cpp src/board.cpp src/book.cpp src/eval.cpp src/init.cpp \
	src/main.cpp src/move.cpp src/movegen.cpp src/piece.cpp src/search.cpp \
	src/transposition.cpp src/xboard.cpp src/zobrist.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=purplehaze

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -pg -o $@ $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<
