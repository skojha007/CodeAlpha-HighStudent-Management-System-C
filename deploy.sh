#!/bin/bash
# Deploy script for Student Management System
# This script compiles and sets up the application for deployment

set -e  # Exit on error

echo "=========================================="
echo "Student Management System - Deploy Script"
echo "=========================================="
echo ""

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check if make is installed
if ! command -v make &> /dev/null; then
    echo -e "${RED}Error: 'make' is not installed${NC}"
    echo "Install it with: sudo apt-get install build-essential"
    exit 1
fi

# Check if gcc is installed
if ! command -v gcc &> /dev/null; then
    echo -e "${RED}Error: 'gcc' is not installed${NC}"
    echo "Install it with: sudo apt-get install gcc"
    exit 1
fi

echo -e "${BLUE}[1/4] Cleaning previous builds...${NC}"
make clean

echo -e "${BLUE}[2/4] Compiling application...${NC}"
make build

echo -e "${BLUE}[3/4] Creating deployment directory...${NC}"
mkdir -p deployment
cp build/student_mgmt deployment/
cp README.md deployment/
cp LICENSE 2>/dev/null || echo "No LICENSE file found"

echo -e "${BLUE}[4/4] Creating startup script...${NC}"
cat > deployment/run.sh << 'EOF'
#!/bin/bash
# Run the Student Management System

cd "$(dirname "$0")"
./student_mgmt
EOF

chmod +x deployment/run.sh

echo ""
echo -e "${GREEN}=========================================="
echo "Deployment Complete!"
echo "==========================================${NC}"
echo ""
echo -e "📂 Deployment directory: ${BLUE}./deployment/${NC}"
echo -e "📄 Executable: ${BLUE}./deployment/student_mgmt${NC}"
echo -e "🚀 Run with: ${BLUE}./deployment/run.sh${NC}"
echo ""
echo "Available commands:"
echo "  • make run       - Build and run immediately"
echo "  • make install   - Install system-wide"
echo "  • make clean     - Clean build files"
echo ""
