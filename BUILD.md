# Build Instructions

Detailed instructions for building PatternCAD from source.

## Prerequisites

### All Platforms

1. **Qt 6.2 or later**
   - Download from: https://www.qt.io/download-open-source
   - Required modules: Core, Gui, Widgets, Svg, PrintSupport

2. **CMake 3.16 or later**
   - Download from: https://cmake.org/download/

3. **Eigen 3.3 or later**
   - Linear algebra library for constraint solver

4. **C++17 compatible compiler**
   - GCC 7+, Clang 5+, MSVC 2017+, or Apple Clang 10+

### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-svg-dev \
    libeigen3-dev \
    git
```

### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake qt@6 eigen git
```

### Windows

1. Install Visual Studio 2019 or later with C++ development tools
2. Download and install Qt 6.2+ from qt.io
3. Download and install CMake from cmake.org
4. Download Eigen from eigen.tuxfamily.org and extract to a known location

## Building

### Linux / macOS

```bash
# Clone repository
git clone <repository-url>
cd PatternCAD

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build (use -j for parallel compilation)
cmake --build . -j4

# Run
./PatternCAD
```

### Windows (Command Prompt)

```cmd
REM Clone repository
git clone <repository-url>
cd PatternCAD

REM Create build directory
mkdir build
cd build

REM Configure (adjust paths as needed)
cmake .. -G "Visual Studio 17 2022" ^
    -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2019_64" ^
    -DEIGEN3_INCLUDE_DIR="C:\path\to\eigen"

REM Build
cmake --build . --config Release

REM Run
Release\PatternCAD.exe
```

### Windows (Qt Creator)

1. Open Qt Creator
2. File → Open File or Project
3. Select `CMakeLists.txt` from PatternCAD directory
4. Configure project with desired kit
5. Build → Build Project
6. Run → Run

## Build Options

### Debug Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Release Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Build with Tests

```bash
cmake .. -DBUILD_TESTING=ON
cmake --build .
ctest
```

## Installation

```bash
# Linux/macOS
sudo cmake --install .

# Windows (run as Administrator)
cmake --install .
```

Default install locations:
- Linux: `/usr/local/bin/PatternCAD`
- macOS: `/Applications/PatternCAD.app`
- Windows: `C:\Program Files\PatternCAD\PatternCAD.exe`

## Troubleshooting

### Qt not found

Ensure Qt is in your PATH or set CMAKE_PREFIX_PATH:
```bash
cmake .. -DCMAKE_PREFIX_PATH="/path/to/Qt/6.x.x/gcc_64"
```

### Eigen not found

Set EIGEN3_INCLUDE_DIR:
```bash
cmake .. -DEIGEN3_INCLUDE_DIR="/path/to/eigen3"
```

### Compiler errors

Ensure you have a C++17 compatible compiler:
```bash
# Check GCC version
gcc --version  # Should be 7.0 or later

# Check Clang version
clang --version  # Should be 5.0 or later
```

## Development Setup

### Qt Creator

1. Install Qt Creator (comes with Qt installer)
2. Open CMakeLists.txt as project
3. Configure with Qt 6 kit
4. Build and run

### Visual Studio Code

1. Install extensions:
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)
   - Qt Tools

2. Open folder in VS Code
3. Configure CMake via CMake Tools
4. Build and debug using CMake Tools

### CLion

1. Open folder as CMake project
2. CLion will automatically detect CMakeLists.txt
3. Configure project
4. Build and run

## Running Tests

```bash
# Build with tests enabled
cmake .. -DBUILD_TESTING=ON
cmake --build .

# Run all tests
ctest

# Run specific test
ctest -R GeometryTests

# Verbose output
ctest -V
```

## Creating Packages

### Linux (DEB package)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
cpack -G DEB
```

### macOS (DMG)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
macdeployqt PatternCAD.app -dmg
```

### Windows (Installer)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cpack -G NSIS
```

### Linux (RPM package)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
cpack -G RPM
```

### Linux (AppImage)

```bash
# Run the build script from project root
./packaging/appimage/build-appimage.sh
```

This creates a portable AppImage that can run on any recent Linux distribution without installation.

### Windows (Portable ZIP)

```powershell
# Run the deployment script from project root
.\packaging\windows\deploy-windows.ps1
```

This creates a portable ZIP package with all dependencies bundled.

## Using vcpkg for Dependencies

vcpkg provides cross-platform dependency management:

```bash
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh  # or bootstrap-vcpkg.bat on Windows

# Build PatternCAD with vcpkg
cd /path/to/PatternCAD
cmake -B build -S . \
    -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The `vcpkg.json` manifest file in the project root automatically handles Qt6 and Eigen3 dependencies.

## Continuous Integration

PatternCAD uses GitHub Actions for CI/CD:

- **Linux builds**: DEB, RPM, and AppImage packages
- **Windows builds**: NSIS installer and portable ZIP
- **Automated testing**: Runs on every commit
- **Release packaging**: Automatic artifact creation for releases

See `.github/workflows/build.yml` for the full CI configuration.
