#!/bin/bash
# PatternCAD Build Script

set -e  # Exit on error

echo "🔨 Building PatternCAD..."
echo ""

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "📁 Creating build directory..."
    mkdir build
fi

cd build

# Configure if needed
if [ ! -f "CMakeCache.txt" ]; then
    echo "⚙️  Configuring CMake..."
    cmake ..
fi

# Build
echo "🔧 Compiling..."
cmake --build . -j$(nproc)

echo ""
echo "✅ Build complete!"
echo ""
echo "📦 Executable: build/PatternCAD"
echo "🚀 Run with: ./build/PatternCAD"
echo ""
