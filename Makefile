optimize = yes
profiler = no
null_move_pruning = no
transpositions_table = yes

CXX = g++
CXXFLAGS = -Wall -pedantic-errors -g -std=c++0x \
	   -Wcast-qual -Wshadow \
	   -W -pipe -Wextra \
	   #-Wconversion

ifeq ($(optimize),yes)
    CXXFLAGS += -O3 -march=native -mtune=native
endif

ifeq ($(profiler),yes)
    CXXFLAGS += -pg -lprofiler
endif

ifeq ($(null_move_pruning),yes)
    CXXFLAGS += -DNMP
endif

ifeq ($(transpositions_table),yes)
    CXXFLAGS += -DTT
endif

SOURCES = $(shell ls src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = purplehaze

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<
