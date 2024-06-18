# Compiler
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Executable
EXEC = parser

# Source files
SRC = parser.cc lexer.cc inputbuf.cc

# Header files
HEADERS = parser.h lexer.h inputbuf.h

# Object files
OBJ = $(SRC:.cc=.o)

# Default target
all: $(EXEC)

# Build the executable
$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(EXEC) $(OBJ)

# Phony targets
.PHONY: all clean
