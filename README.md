# PatternCAD

A professional desktop application for creating parametric sewing patterns with constraint-based design.

## Features

- **Parametric Design**: Create patterns with adjustable parameters
- **Constraint System**: Maintain geometric relationships automatically
- **Professional Tools**: Seam allowances, notches, grading, nesting
- **File Interoperability**: Import/export DXF, SVG, PDF formats
- **Cross-Platform**: Runs on Windows, macOS, and Linux

## Project Status

Current Phase: **Initial Development**
- ✅ Product Brief
- ✅ Product Requirements Document (PRD)
- ✅ Architecture Document
- ✅ Sprint Planning (8 epics, 61 stories)
- 🚧 Implementation (In Progress)

## Building from Source

### Prerequisites

- **Qt 6.2+** (Core, Widgets, Svg, PrintSupport modules)
- **CMake 3.16+**
- **C++17 compatible compiler**
- **Eigen 3.3+** (Linear algebra library)

### Build Instructions

#### Linux / macOS

```bash
# Clone the repository
git clone <repository-url>
cd PatternCAD

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./PatternCAD
```

#### Windows (Visual Studio)

```cmd
# Create build directory
mkdir build
cd build

# Configure for Visual Studio
cmake .. -G "Visual Studio 17 2022"

# Build
cmake --build . --config Release

# Run
Release\PatternCAD.exe
```

## Project Structure

```
PatternCAD/
├── docs/                      # Documentation
│   ├── product-brief.md
│   ├── prd-PatternCAD-*.md
│   ├── architecture-*.md
│   ├── sprint-status.yaml
│   └── stories/              # User stories by epic
├── src/                      # Source code
│   ├── core/                 # Core application logic
│   ├── ui/                   # User interface components
│   ├── geometry/             # Geometric primitives
│   ├── tools/                # Drawing and editing tools
│   ├── constraints/          # Constraint solver
│   └── io/                   # File I/O operations
├── tests/                    # Unit and integration tests
├── resources/                # Icons, styles, assets
├── third-party/              # External dependencies
└── CMakeLists.txt           # Build configuration
```

## Documentation

- [Product Brief](docs/product-brief.md)
- [Product Requirements Document](docs/prd-PatternCAD-2026-01-27.md)
- [Architecture Document](docs/architecture-PatternCAD-2026-01-27.md)
- [Sprint Planning](docs/sprint-status.yaml)
- [User Stories](docs/stories/)

## Development Roadmap

See [Sprint Status](docs/sprint-status.yaml) for detailed epic and story breakdown.

### Current Focus (Sprint 0)
- Project skeleton setup ✅
- Build system configuration ✅
- Basic application window
- Core geometry classes
- File format design

## Contributing

This project follows the BMAD Method for structured software development.

1. Check [Sprint Status](docs/sprint-status.yaml) for available stories
2. Review story files in `docs/stories/` for requirements
3. Follow the architecture guidelines in the Architecture Document
4. Write tests for all new functionality
5. Submit pull requests with story references

## License

[License information to be added]

## Contact

[Contact information to be added]
