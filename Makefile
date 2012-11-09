include configure.mk

SOURCES = $(shell ls src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = purplehaze

.PHONY: all test clean

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test:
	$(MAKE) -C ./test/unit
	$(MAKE) -C ./test/unit test

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
	$(MAKE) -C ./test/unit clean
