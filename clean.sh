#!/bin/bash
# PatternCAD Clean Script

echo "🧹 Cleaning build artifacts..."

if [ -d "build" ]; then
    rm -rf build
    echo "✅ Build directory removed"
fi

# Clean Qt generated files
find . -name "moc_*.cpp" -delete
find . -name "ui_*.h" -delete
find . -name "qrc_*.cpp" -delete

# Clean CMake files
find . -name "CMakeCache.txt" -delete
find . -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true

echo "✅ Clean complete!"
