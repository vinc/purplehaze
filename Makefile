print_warnings = yes
revision_number = yes
optimize = yes
profiler = no
gdb = no
null_move_pruning = yes
debug = no

ifndef compiler
    compiler = gcc
endif
ifeq ($(compiler),gcc)
    CXX = g++
endif
ifeq ($(compiler),intel)
    CXX = icpc
endif
ifeq ($(compiler),clang)
    CXX = clang++
endif

CXXFLAGS = -std=c++0x -pipe

ifeq ($(print_warnings),yes)
    ifeq ($(compiler),intel)
        CXXFLAGS += -Wall -Wremarks -wd981 -wd2259
    else
        CXXFLAGS += -Wall -pedantic-errors -Wcast-qual -Wshadow -Wextra
    endif
endif

ifeq ($(revision_number),yes)
    CXXFLAGS += -DVERSION=\"$(shell git describe HEAD)\"
endif

ifeq ($(gdb),yes)
    CXXFLAGS += -g
endif

ifeq ($(optimize),yes)
    ifeq ($(compiler),intel)
        CXXFLAGS += -fast
    else
        CXXFLAGS += -O3 -march=native -mtune=native
    endif
endif

ifeq ($(profiler),yes)
    CXXFLAGS += -pg -lprofiler
endif

ifeq ($(null_move_pruning),yes)
    CXXFLAGS += -DNMP
endif

ifeq ($(debug),no)
    CXXFLAGS += -DNDEBUG
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
