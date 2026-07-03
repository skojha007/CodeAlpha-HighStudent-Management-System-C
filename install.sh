#!/bin/bash
# Installation script for Student Management System
# Run with: sudo bash install.sh

set -e

echo "========================================"
echo "Student Management System - Installer"
echo "========================================"
echo ""

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
    echo -e "${RED}Please run as root (sudo bash install.sh)${NC}"
    exit 1
fi

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
else
    echo -e "${RED}Unsupported OS: $OSTYPE${NC}"
    exit 1
fi

echo -e "${BLUE}Detected OS: $OS${NC}"
echo ""

# Install dependencies
echo -e "${BLUE}Installing dependencies...${NC}"
if [ "$OS" == "linux" ]; then
    if command -v apt-get &> /dev/null; then
        apt-get update
        apt-get install -y build-essential gcc make
    elif command -v dnf &> /dev/null; then
        dnf install -y gcc make
    fi
elif [ "$OS" == "macos" ]; then
    if ! command -v brew &> /dev/null; then
        echo -e "${RED}Homebrew not found. Please install from https://brew.sh${NC}"
        exit 1
    fi
    brew install gcc make
fi

echo -e "${GREEN}✓ Dependencies installed${NC}"
echo ""

# Build the project
echo -e "${BLUE}Building Student Management System...${NC}"
make clean
make build

echo -e "${GREEN}✓ Build successful${NC}"
echo ""

# Install binary
echo -e "${BLUE}Installing to /usr/local/bin/...${NC}"
install -m 0755 build/student_mgmt /usr/local/bin/student_mgmt

echo -e "${GREEN}✓ Installation complete${NC}"
echo ""

echo "========================================"
echo -e "${GREEN}Installation Successful!${NC}"
echo "========================================"
echo ""
echo -e "Run: ${BLUE}student_mgmt${NC}"
echo ""
echo "To uninstall:"
echo -e "  ${BLUE}sudo rm /usr/local/bin/student_mgmt${NC}"
echo ""
