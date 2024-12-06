# Compiler
CXX = clang++

# TODO: move disable -g if you need prod mode
# Compiler flags
# Use -O3 for production or benchmarking builds, and -g for debugging
CXXFLAGS_DEBUG = -Wall -Wextra -std=c++20 -g
CXXFLAGS_OPTIMIZED = -Wall -Wextra -std=c++20 -O2

# Include and library paths
INCLUDE_DIRS = -I dependencies/include -I /opt/homebrew/Cellar/glm/1.0.1/include
LIB_DIRS = -L dependencies/library
LIBS = dependencies/library/libglfw.3.4.dylib
FRAMEWORKS = -framework OpenGL -framework Cocoa -framework IOKit \
       -framework CoreVideo -framework CoreFoundation

# Source files (all .cpp files in the directory)
SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h) $(wildcard UnitLib/*.h) $(wildcard _Tests/*.h) $(wildcard PhysicsLib/*.h)

# Primary targets
TARGET_MAIN = main
TARGET_TESTS = _Tests/tests
TARGET_BENCHMARK = benchmark

# Object files (replace .cpp with .o for each source file)
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
# Rule to link object files into the executable
$(TARGET_MAIN): $(OBJECTS) $(HEADERS) glad.o
	$(CXX) $(CXXFLAGS_OPTIMIZED) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(FRAMEWORKS) $(OBJECTS) glad.o -o $(TARGET_MAIN)

tests: $(TARGET_TESTS).o $(HEADERS)
	$(CXX) $(CXXFLAGS_OPTIMIZED) $(TARGET_TESTS).o -o $(TARGET_TESTS)

benchmark: benchmark.o glad.o
	$(CXX) $(CXXFLAGS_OPTIMIZED) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(FRAMEWORKS) benchmark.o glad.o -o $(TARGET_BENCHMARK)

benchmark.o: benchmark.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_OPTIMIZED) $(INCLUDE_DIRS) -c benchmark.cpp -o benchmark.o

# Rule to compile .cpp files into .o files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_OPTIMIZED) $(INCLUDE_DIRS) -c $< -o $@

$(TARGET_TESTS).o: $(HEADERS) $(TARGET_TESTS).cpp
	$(CXX) $(CXXFLAGS_OPTIMIZED) -c $(TARGET_TESTS).cpp -o $(TARGET_TESTS).o

glad.o:
	clang $(INCLUDE_DIRS) -c glad.c -o glad.o

# Clean rule to remove generated files
clean:
	rm -f $(OBJECTS) $(TARGET_MAIN) glad.o $(TARGET_BENCHMARK)
	rm -f _Tests/*.o $(TARGET_TESTS)

# Phony targets to prevent conflicts with files named 'clean' or 'all'
.PHONY: clean
