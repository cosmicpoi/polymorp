# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++20

# Source files (all .cpp files in the directory)
SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)

# Object files (replace .cpp with .o for each source file)
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
# Rule to link object files into the executable
main: $(OBJECTS) $(HEADERS)
	$(CXX) $(OBJECTS) -o main

# TODO: Right now we recompile the whole thing whenever a header changes, there should be a smarter way to do this incrementally
# Rule to compile .cpp files into .o files
# %.o: %.cpp
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(OBJECTS) main

# Phony targets to prevent conflicts with files named 'clean' or 'all'
.PHONY: all clean