# Makefile for Raytracing Project

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -I./include
LDFLAGS = -lGL -lGLU -lglut -lm

# Directories
SRC_DIR = src
BIN_DIR = bin/x64
OBJ_DIR = obj

# Target executable
TARGET = raytracer

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories if they don't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Clean complete"

# Clean and rebuild
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET) scene1.txt scene1.ppm

# Show variables (for debugging)
debug:
	@echo "CXX      = $(CXX)"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "SOURCES  = $(SOURCES)"
	@echo "OBJECTS  = $(OBJECTS)"
	@echo "TARGET   = $(TARGET)"

.PHONY: all clean rebuild run debug
