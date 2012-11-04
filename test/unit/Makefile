include ../../configure.mk

CXXFLAGS += -lgtest

SOURCES = $(shell find ../.. -name '*.cpp' ! -name 'main.cpp' | sort)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = test_purplehaze

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test:
	./$(EXECUTABLE)

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
