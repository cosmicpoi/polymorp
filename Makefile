# Compiler
CXX = clang++

# TODO: move disable -g if you need prod mode
# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++20 -g
INCLUDE_DIRS = -I dependencies/include
LIB_DIRS = -L dependencies/library
LIBS = dependencies/library/libglfw.3.4.dylib \
       -framework OpenGL -framework Cocoa -framework IOKit \
       -framework CoreVideo -framework CoreFoundation

# Source files (all .cpp files in the directory)
SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h) $(wildcard UnitLib/*.h) $(wildcard _Tests/*.h)


# Primary targets
TARGET_MAIN = main
TARGET_TESTS = _Tests/tests

# Object files (replace .cpp with .o for each source file)
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
# Rule to link object files into the executable
$(TARGET_MAIN): $(OBJECTS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(OBJECTS) glad.c -o $(TARGET_MAIN)

tests: $(TARGET_TESTS).o $(HEADERS)
	$(CXX) $(CXXFLAGS) $(TARGET_TESTS).o -o $(TARGET_TESTS)

# TODO: Right now we recompile the whole thing whenever a header changes, there should be a smarter way to do this incrementally
# Rule to compile .cpp files into .o files
# %.o: %.cpp
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) -c $< -o $@

$(TARGET_TESTS).o: $(HEADERS) $(TARGET_TESTS).cpp
	$(CXX) $(CXXFLAGS) -c $(TARGET_TESTS).cpp -o $(TARGET_TESTS).o

# Clean rule to remove generated files
clean:
	rm -f $(OBJECTS) $(TARGET_MAIN)
	rm -f _Tests/*.o $(TARGET_TESTS)

# Phony targets to prevent conflicts with files named 'clean' or 'all'
.PHONY: clean