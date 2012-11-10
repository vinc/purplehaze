include ../../configure.mk

CXXFLAGS += -I ${GTEST}/include
GTEST = 'gtest-1.6.0'

SOURCES = $(shell find ../.. -name '*.cpp' ! -name 'main.cpp' | sort)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = test_purplehaze

.PHONY: all test clean

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): libgtest.a $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) libgtest.a

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

libgtest.a:
	curl -s -C - http://googletest.googlecode.com/files/$(GTEST).zip > $(GTEST).zip
	unzip $(GTEST).zip
	$(CXX) -I ${GTEST}/include -I ${GTEST} $(CXXFLAGS) -c ${GTEST}/src/gtest-all.cc
	$(AR) -rv libgtest.a gtest-all.o

test:
	./$(EXECUTABLE)

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
	$(RM) -r *gtest*
