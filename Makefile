# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++20

# Source files (all .cpp files in the directory)
SOURCES = $(wildcard *.cpp)

# Object files (replace .cpp with .o for each source file)
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
# Rule to link object files into the executable
main: $(OBJECTS)
	$(CXX) $(OBJECTS) -o main

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(OBJECTS) main

# Phony targets to prevent conflicts with files named 'clean' or 'all'
.PHONY: all clean