CXX=g++
CXXFLAGS=-Wall -pedantic-errors -g -O3 -march=native -mtune=native
SOURCES=src/attack.cpp src/board.cpp src/book.cpp src/eval.cpp src/init.cpp \
	src/main.cpp src/move.cpp src/movegen.cpp src/piece.cpp src/search.cpp \
	src/transposition.cpp src/xboard.cpp src/zobrist.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=purplehaze

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<
