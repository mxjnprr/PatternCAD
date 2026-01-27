#!/bin/bash
# PatternCAD Run Script

if [ ! -f "build/PatternCAD" ]; then
    echo "❌ PatternCAD executable not found!"
    echo "🔨 Building first..."
    ./build.sh
fi

echo "🚀 Launching PatternCAD..."
./build/PatternCAD "$@"
