# Makefile for Student Management System
# Compilation and management commands

.PHONY: all build clean run test install help

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -O2
DEBUG_FLAGS := -Wall -Wextra -std=c11 -g -DDEBUG

# Target executable
TARGET := student_mgmt
SOURCE := main.c
OBJECT := main.o

# Installation directory
INSTALL_DIR := /usr/local/bin
BUILD_DIR := build
DIST_DIR := dist

all: clean build

# Build the project
build: $(TARGET)
	@echo "✓ Build successful: $(TARGET)"

$(TARGET): $(SOURCE)
	@echo "Compiling $(SOURCE)..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(SOURCE)
	@cp $(BUILD_DIR)/$(TARGET) ./

# Debug build with symbols
debug: $(SOURCE)
	@echo "Building debug version..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(DEBUG_FLAGS) -o $(BUILD_DIR)/$(TARGET)_debug $(SOURCE)
	@echo "✓ Debug build: $(BUILD_DIR)/$(TARGET)_debug"

# Run the program
run: $(TARGET)
	@echo "Starting Student Management System..."
	@./$(TARGET)

# Clean build artifacts
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR)/ $(DIST_DIR)/ *.o
	@rm -f $(TARGET)
	@echo "✓ Clean complete"

# Install to system (requires sudo)
install: $(TARGET)
	@echo "Installing $(TARGET) to $(INSTALL_DIR)..."
	@sudo mkdir -p $(INSTALL_DIR)
	@sudo cp $(BUILD_DIR)/$(TARGET) $(INSTALL_DIR)/
	@sudo chmod +x $(INSTALL_DIR)/$(TARGET)
	@echo "✓ Installation complete. Run 'student_mgmt' from anywhere."

# Uninstall from system
uninstall:
	@echo "Uninstalling $(TARGET)..."
	@sudo rm -f $(INSTALL_DIR)/$(TARGET)
	@echo "✓ Uninstalled"

# Create distribution package
dist: clean build
	@echo "Creating distribution package..."
	@mkdir -p $(DIST_DIR)
	@cp $(BUILD_DIR)/$(TARGET) $(DIST_DIR)/
	@cp README.md $(DIST_DIR)/
	@cp LICENSE 2>/dev/null || echo "No LICENSE file"
	@tar -czf $(TARGET)-release.tar.gz $(DIST_DIR)/
	@echo "✓ Distribution: $(TARGET)-release.tar.gz"

# Help
help:
	@echo "Student Management System - Build Commands"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Clean and build (default)"
	@echo "  build     - Compile the program"
	@echo "  debug     - Build with debug symbols"
	@echo "  run       - Build and run the program"
	@echo "  clean     - Remove build artifacts"
	@echo "  install   - Install system-wide (requires sudo)"
	@echo "  uninstall - Remove system installation"
	@echo "  dist      - Create distribution package"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Example:"
	@echo "  make build && make run"
	@echo "  make install  # Install globally"
