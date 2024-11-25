# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++20

# Source files (all .cpp files in the directory)
SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h) $(wildcard UnitLib/*.h)

# Primary targets
TARGET_MAIN = main
TARGET_TESTS = _Tests/tests

# Object files (replace .cpp with .o for each source file)
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
# Rule to link object files into the executable
main: $(OBJECTS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET_MAIN)

tests: _Tests/tests.o $(HEADERS)
	$(CXX) $(CXXFLAGS) _Tests/tests.o -o $(TARGET_TESTS)

# TODO: Right now we recompile the whole thing whenever a header changes, there should be a smarter way to do this incrementally
# Rule to compile .cpp files into .o files
# %.o: %.cpp
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

tests.o:
	$(CXX) $(CXXFLAGS) -c _Tests/tests.cpp -o -Tests/tests.o

# Clean rule to remove generated files
clean:
	rm -f $(OBJECTS) main

# Phony targets to prevent conflicts with files named 'clean' or 'all'
.PHONY: clean