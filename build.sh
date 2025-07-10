#!/bin/bash

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}=== DistCache Universal Build Script ===${NC}"

# Clean previous builds
echo "🧹 Cleaning previous builds..."
rm -rf build/
rm -f distcache run_benchmark run_tests

# Check available build systems
HAS_CMAKE=false
HAS_MAKE=false

if command -v cmake &> /dev/null; then
    HAS_CMAKE=true
    echo -e "${GREEN}✅ CMake available${NC}"
else
    echo -e "${YELLOW}⚠️  CMake not found${NC}"
fi

if command -v make &> /dev/null; then
    HAS_MAKE=true
    echo -e "${GREEN}✅ Make available${NC}"
else
    echo -e "${YELLOW}⚠️  Make not found${NC}"
fi

# Build with best available system
if [ "$HAS_CMAKE" = true ]; then
    echo -e "${GREEN}🔨 Building with CMake...${NC}"
    mkdir -p build
    cd build
    
    # Configure
    if cmake -DCMAKE_BUILD_TYPE=Release ..; then
        echo -e "${GREEN}✅ CMake configuration successful${NC}"
    else
        echo -e "${RED}❌ CMake configuration failed${NC}"
        exit 1
    fi
    
    # Build
    if make -j$(nproc); then
        echo -e "${GREEN}✅ CMake build successful${NC}"
    else
        echo -e "${RED}❌ CMake build failed${NC}"
        exit 1
    fi
    
    cd ..
    echo "Built executables:"
    ls -la build/distcache build/run_benchmark build/run_tests 2>/dev/null || echo "Some executables may not have been built"
    
elif [ "$HAS_MAKE" = true ]; then
    echo -e "${GREEN}🔨 Building with Makefile...${NC}"
    if make -j$(nproc); then
        echo -e "${GREEN}✅ Makefile build successful${NC}"
    else
        echo -e "${RED}❌ Makefile build failed${NC}"
        exit 1
    fi
    
    echo "Built executables:"
    ls -la distcache run_benchmark run_tests 2>/dev/null || echo "Some executables may not have been built"
    
else
    echo -e "${RED}❌ Neither CMake nor Make found. Please install one of them.${NC}"
    echo "Ubuntu/Debian: sudo apt-get install cmake make build-essential"
    echo "macOS: brew install cmake make"
    exit 1
fi

echo ""
echo -e "${GREEN}🎉 Build completed successfully!${NC}"
echo ""
echo "Available commands:"
if [ -f "build/distcache" ]; then
    echo "  ./build/distcache      - Start cache server"
    echo "  ./build/run_tests      - Run test suite"
    echo "  ./build/run_benchmark  - Run performance benchmarks"
elif [ -f "distcache" ]; then
    echo "  ./distcache            - Start cache server"
    echo "  ./run_tests            - Run test suite"
    echo "  ./run_benchmark        - Run performance benchmarks"
fi
echo ""
echo "Next steps:"
echo "  1. Run tests:      ./build/run_tests (or ./run_tests)"
echo "  2. Run benchmarks: ./build/run_benchmark (or ./run_benchmark)"
echo "  3. Start server:   ./build/distcache (or ./distcache)"
echo "  4. Docker deploy:  docker-compose up -d"