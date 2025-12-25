#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Configurations
BUILD_DIR="build"
ENABLE_TESTS=false
RUN_FORMAT=false

# Parse arguments
for arg in "$@"; do
    case $arg in
        --format)
            RUN_FORMAT=true
            ;;
        *)
            echo "Unknown option: $arg"
            exit 1
            ;;
    esac
done

echo "=== Starting Build Process ==="

# Clean existing build
if [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
echo "Creating build directory..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake configuration
echo "Running CMake configuration..."
cmake .. -DENABLE_TESTS=${ENABLE_TESTS}

if [ "$RUN_FORMAT" = true ]; then
    echo "Running format target..."
    cmake --build . --target format
fi

# Build the project
echo "Building project..."
cmake --build .

echo ""
echo "✓ Build Successful"
echo "Run the compiler with: ./build/neko <source-file>"
echo ""
echo "To assemble and link the output:"
echo "  nasm -f elf64 output.asm -o output.o"
echo "  gcc -no-pie output.o -o output"
echo "  ./output"
