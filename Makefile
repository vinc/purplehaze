CXX=g++
CXXFLAGS=-Wall -pedantic-errors -g -O3 -march=native -mtune=native -std=c++0x
#CXXFLAGS=-Wall -pedantic-errors -pg -std=c++0x
#CXXFLAGS=-Wall -pedantic-errors -g -std=c++0x #-lprofiler
SOURCES=$(shell ls src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=purplehaze

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<
