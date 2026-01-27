# System Architecture: PatternCAD

**Date:** 2026-01-27
**Architect:** System Architect
**Version:** 1.0
**Project Type:** desktop-app
**Project Level:** 3
**Status:** Draft

---

## Document Overview

This document defines the system architecture for PatternCAD. It provides the technical blueprint for implementation, addressing all functional and non-functional requirements from the PRD.

**Related Documents:**
- Product Requirements Document: docs/prd-PatternCAD-2026-01-27.md
- Product Brief: docs/product-brief.md

---

## Executive Summary

PatternCAD is a professional-grade 2D pattern design application built with a **Linux-first, cross-platform** philosophy. The architecture is designed around **performance, scalability to 300+ pattern pieces, and cross-platform consistency**.

**Key Architectural Decisions:**

1. **Qt 6 (C++) Framework** - Native performance, excellent cross-platform support, proven for professional CAD applications
2. **Layered Architecture** - Clear separation between UI, business logic, domain model, and infrastructure
3. **Command Pattern** - Comprehensive undo/redo system supporting all operations
4. **Scene Graph Rendering** - Qt Graphics View for efficient 2D vector rendering with isolation mode optimization
5. **Custom Constraint Solver** - Geometric constraint system for parametric design
6. **Modular File I/O** - Plugin-based import/export for DXF, SVG, PDF formats

The architecture addresses all 27 functional requirements and 17 non-functional requirements, with particular focus on performance (NFR-001, NFR-002), scalability to hundreds of pieces (NFR-006), and true cross-platform consistency (NFR-011, FR-025).

---

## Architectural Drivers

These requirements heavily influence architectural decisions:

### Critical Functional Drivers

**FR-003: Spacebar Grab-and-Move with Real-Time Rendering**
- Requires high-performance rendering engine (60fps)
- Low-latency input handling (< 16ms)
- **Drives:** Graphics engine choice, input event architecture

**FR-004/FR-005: Parametric Design and Constraints**
- Complex dependency management
- Real-time constraint solving
- Circular dependency detection
- **Drives:** Domain model design, constraint solver architecture

**FR-027: Isolation Mode for 300-500 Pieces**
- Scene culling and selective rendering
- Fast mode switching (< 100ms)
- **Drives:** Rendering architecture, scene management

**FR-014: Automatic Nesting**
- Computationally intensive optimization
- Progress reporting and cancellation
- **Drives:** Multi-threading architecture, algorithm design

**FR-018/FR-019: Multi-Format Import/Export**
- Multiple file format parsers
- Format translation layer
- **Drives:** Plugin architecture, data model abstraction

### Critical Non-Functional Drivers

**NFR-001/NFR-002: Performance (60fps, 300+ pieces)**
- Efficient rendering pipeline
- Spatial indexing for large scenes
- **Drives:** Graphics engine, data structures

**NFR-006: Scalability (300-500 pieces)**
- Memory-efficient data structures
- Lazy loading and rendering
- **Drives:** Scene graph design, isolation mode implementation

**NFR-011/FR-025: Cross-Platform Consistency**
- Single codebase for Linux and Windows
- Identical behavior and file format
- **Drives:** Framework choice (Qt), build system

**NFR-012/NFR-013: Maintainability and Testing**
- Clean architecture with separation of concerns
- Testable components
- **Drives:** Layered architecture, dependency injection

---

## System Overview

### High-Level Architecture

PatternCAD follows a **Layered Architecture** pattern with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────┐
│                   PRESENTATION LAYER                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   MainWindow │  │  ToolPalette │  │Properties    │      │
│  │   Canvas     │  │  LayersPanel │  │Parameters    │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│          Qt Widgets, Qt Graphics View, Event Handlers       │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                   APPLICATION LAYER                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  Commands    │  │  Use Cases   │  │  Services    │      │
│  │  (Undo/Redo) │  │              │  │              │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│     CreateLineCommand, MoveObjectCommand, etc.              │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                     DOMAIN LAYER                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   Geometry   │  │  Parametric  │  │  Constraint  │      │
│  │   Objects    │  │   System     │  │   Solver     │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   Pattern    │  │    Layer     │  │   Project    │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│         Core business logic, no UI dependencies             │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                 INFRASTRUCTURE LAYER                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  File I/O    │  │  Persistence │  │  Algorithms  │      │
│  │  (DXF,SVG,   │  │  (Native     │  │  (Nesting,   │      │
│  │   PDF)       │  │   Format)    │  │   Offset)    │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
```

### Architecture Diagram

```
                         PatternCAD Application
                                  │
                ┌─────────────────┼─────────────────┐
                │                 │                 │
         ┌──────▼──────┐   ┌─────▼─────┐   ┌──────▼──────┐
         │   Canvas    │   │  Panels   │   │   Menus     │
         │  Rendering  │   │  (Props,  │   │  Toolbars   │
         │             │   │  Layers)  │   │             │
         └──────┬──────┘   └─────┬─────┘   └──────┬──────┘
                │                 │                 │
                └─────────────────┼─────────────────┘
                                  │
                        ┌─────────▼─────────┐
                        │  Command Manager  │
                        │  (Undo/Redo Stack)│
                        └─────────┬─────────┘
                                  │
                ┌─────────────────┼─────────────────┐
                │                 │                 │
         ┌──────▼──────┐   ┌─────▼─────┐   ┌──────▼──────┐
         │   Pattern   │   │ Parametric│   │  Constraint │
         │   Model     │   │  Engine   │   │   Solver    │
         │             │   │           │   │             │
         └──────┬──────┘   └─────┬─────┘   └──────┬──────┘
                │                 │                 │
                └─────────────────┼─────────────────┘
                                  │
                        ┌─────────▼─────────┐
                        │   Geometry Core   │
                        │  (Points, Lines,  │
                        │   Curves, etc.)   │
                        └─────────┬─────────┘
                                  │
                ┌─────────────────┼─────────────────┐
                │                 │                 │
         ┌──────▼──────┐   ┌─────▼─────┐   ┌──────▼──────┐
         │  DXF/SVG    │   │  Native   │   │  Nesting    │
         │  Import/    │   │  File     │   │  Algorithm  │
         │  Export     │   │  Format   │   │             │
         └─────────────┘   └───────────┘   └─────────────┘
```

### Architectural Pattern

**Pattern:** Layered Architecture with Command Pattern and Model-View separation

**Rationale:**

1. **Layered Architecture:**
   - **Clear separation of concerns** - UI (Qt), business logic, domain model, infrastructure
   - **Testability** - Each layer can be tested independently
   - **Maintainability** - Changes isolated to specific layers
   - **Flexibility** - Can swap UI or persistence without affecting domain

2. **Command Pattern:**
   - **Comprehensive Undo/Redo** (FR-024) - All operations encapsulated as commands
   - **Macro operations** - Group multiple commands into single undo step
   - **Transaction support** - Operations can be rolled back on error

3. **Model-View Separation:**
   - **Domain model independent of UI** - Can be tested without Qt
   - **Multiple views possible** - Could add different visualization modes
   - **Observer pattern** - Views update automatically when model changes

4. **Plugin Architecture for File I/O:**
   - **Extensibility** - Easy to add new file formats
   - **Modularity** - File format code isolated from core

---

## Technology Stack

### Core Framework

**Qt 6.5+ (C++17)**

**Selection Rationale:**
- ✓ **Native cross-platform** - Linux and Windows with single codebase (FR-025)
- ✓ **Proven for CAD** - Used by FreeCAD, KiCad, other professional tools
- ✓ **Performance** - Native C++, efficient rendering (NFR-001, NFR-002)
- ✓ **Rich UI widgets** - Professional desktop application UI (FR-021)
- ✓ **Graphics View framework** - Excellent for 2D vector graphics
- ✓ **LGPL license** - Compatible with open-source goals
- ✓ **Excellent documentation** - Large community, mature ecosystem

**Alternatives Considered:**
- ❌ **Electron** - Too heavy, poor performance for 300+ pieces, large memory footprint
- ❌ **GTK** - Weaker Windows support, less suitable for professional CAD UI
- ❌ **wxWidgets** - Less modern, fewer graphics features

**Components Used:**
- **Qt Widgets** - Main window, dialogs, panels, toolbars
- **Qt Graphics View** - Canvas rendering, scene graph (FR-001, FR-002, FR-003)
- **Qt Core** - Event system, file I/O, threading
- **Qt SVG** - SVG import/export (FR-018, FR-019)
- **Qt Concurrent** - Threading for nesting algorithm (FR-014)

### Graphics Rendering

**Qt Graphics View Framework + Optional OpenGL Acceleration**

**Architecture:**
```
QGraphicsScene (Scene Graph)
    ├─ PatternItem (represents a pattern piece)
    │   ├─ GeometryItem (lines, curves, arcs)
    │   ├─ NotchItem (notches, match points)
    │   └─ SeamAllowanceItem (seam allowance outline)
    └─ LayoutItem (layout sheet boundary)

QGraphicsView (Canvas Widget)
    ├─ Handles pan, zoom, input events
    ├─ Renders visible items only (culling)
    └─ Optional OpenGL rendering for acceleration
```

**Performance Optimizations:**
- **Spatial indexing** - BSP tree in QGraphicsScene for fast item lookup
- **View frustum culling** - Only render visible items
- **Level of Detail (LOD)** - Simplify curves at low zoom levels
- **Isolation mode** - Render only selected pieces (FR-027)
- **Caching** - QGraphicsItem cache mode for static items

**Rationale:**
- ✓ Proven for 1000+ objects in scene graph
- ✓ Built-in culling, transforms, selection
- ✓ OpenGL acceleration available if needed
- ✓ Cross-platform (Linux, Windows) with identical rendering

### Geometry Engine

**Custom Geometry Library (C++17) + Boost.Geometry**

**Core Classes:**
- `Point2D` - (x, y) with units
- `Line` - Two points
- `Arc` - Center, radius, start/end angles
- `CubicBezier` - Four control points (FR-002)
- `Polyline` - Sequence of points/curves
- `Shape` - Closed polyline (pattern piece)

**Libraries:**
- **Boost.Geometry** - Spatial operations (intersections, offsets, contains)
- **Eigen** - Linear algebra for transformations, constraint solving
- **Custom code** - Bezier evaluation, arc-length parametrization

**Rationale:**
- ✓ Full control over geometry representations
- ✓ Boost.Geometry provides proven algorithms (seam allowance offset)
- ✓ Eigen widely used in CAD for math operations

### Constraint Solver

**Custom Geometric Constraint Solver**

**Algorithm:** Modified Newton-Raphson with gradient descent fallback

**Constraint Types:**
- Distance (point-to-point, point-to-line)
- Angle (parallel, perpendicular, fixed angle)
- Coincident (points at same location)
- Tangent (curves meeting smoothly)
- Equal (equal length lines, equal radius arcs)

**Implementation Approach:**
- Build constraint graph with dependencies
- Detect circular dependencies (error condition)
- Solve iteratively using numerical methods
- Update geometry when parameters change (observer pattern)

**Alternative Considered:**
- SolveSpace constraint solver (GPL) - Licensing incompatible
- FreeCAD Sketcher (LGPL) - Could adapt, but complex integration

**Rationale:**
- ✓ Full control over solver behavior
- ✓ Can optimize for pattern design use cases
- ✓ License compatible

### Parametric System

**Custom Expression Parser + Dependency Graph**

**Components:**
1. **Expression Parser** - Parse formulas like "waist_width / 2 + seam_allowance"
2. **Dependency Graph** - Track which geometry depends on which parameters
3. **Update Propagation** - When parameter changes, update dependent geometry
4. **Circular Reference Detection** - Prevent infinite loops

**Libraries:**
- **muparser** or **custom recursive descent parser** - Expression evaluation

**Rationale:**
- ✓ Flexible parameter system (FR-004)
- ✓ Efficient updates (only affected geometry recalculated)

### File Format Libraries

**DXF Import/Export:**
- **libdxfrw** (GPL) or **dxflib** (GPL) - DXF R12-R2018 support
- **License Note:** GPL libraries require dynamic linking or re-licensing discussion
- **Alternative:** Custom DXF parser for R12 (simpler format)

**SVG Import/Export:**
- **Qt SVG module** - Built-in, LGPL-compatible
- **Custom path parser** - Convert SVG paths to geometry primitives

**PDF Export:**
- **QPdfWriter** (Qt) - Native PDF generation
- **Alternative:** libharu (ZLIB license) for more control

**Native Format:**
- **JSON** - Human-readable, easy to parse (Qt JSON module)
- **Format:** Geometry + parameters + constraints + metadata
- **Versioning:** Version field for forward/backward compatibility

**Rationale:**
- ✓ Standard formats ensure interoperability (NFR-015)
- ✓ Qt provides several formats built-in
- ✓ JSON native format is debuggable and extensible

### Nesting Algorithm

**Custom Bin Packing Algorithm with Genetic Algorithm Optimization**

**Approach:**
- **Phase 1:** Simple greedy placement (fast, "good enough")
- **Phase 2 (optional):** Genetic algorithm optimization (slower, better results)
- **Features:**
  - Rotation support (0°, 90°, 180°, 270°, arbitrary)
  - Grain direction constraints
  - Spacing/gap handling
  - Non-convex piece support

**Libraries:**
- **Custom implementation** using Qt Concurrent for threading
- **Clipper** (Boost Software License) - Polygon operations, no-fit polygon generation

**Alternative:**
- **SVGNest** (JavaScript) - Would require porting or embedding JS engine
- **Simple heuristics** - Bottom-left, first-fit decreasing

**Rationale:**
- ✓ Custom solution allows phased approach (fast then optimal)
- ✓ Can optimize for pattern design specifically
- ✓ Full control over cancellation and progress

### Build System & Tooling

**CMake 3.20+**

**Build Configuration:**
```cmake
cmake_minimum_required(VERSION 3.20)
project(PatternCAD VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Svg Concurrent)
find_package(Boost REQUIRED COMPONENTS geometry)
find_package(Eigen3 REQUIRED)

# Platform-specific settings
if(LINUX)
    # Linux-specific flags
endif()
if(WIN32)
    # Windows-specific flags
endif()
```

**Package Management:**
- **vcpkg** or **Conan** - C++ dependency management
- **Qt online installer** - Qt framework

**IDE Support:**
- **CLion** - Primary (cross-platform, CMake integration)
- **Qt Creator** - Alternative (Qt-specific features)
- **VSCode** - With C++ and CMake extensions

**Rationale:**
- ✓ CMake standard for cross-platform C++ projects
- ✓ Qt has excellent CMake support
- ✓ vcpkg simplifies dependency management

### Testing Framework

**Google Test (gtest) + Qt Test**

**Test Structure:**
```
tests/
├── unit/              # Unit tests (gtest)
│   ├── geometry/      # Geometry primitives
│   ├── parametric/    # Parametric system
│   └── constraints/   # Constraint solver
├── integration/       # Integration tests (gtest + Qt Test)
│   ├── file_io/       # Import/export
│   └── commands/      # Command system
└── ui/                # UI tests (Qt Test)
    └── interactions/  # User interactions
```

**Coverage Target:** 60% core modules, 40% overall (NFR-013)

**Rationale:**
- ✓ gtest industry standard for C++
- ✓ Qt Test for Qt-specific UI testing
- ✓ Separating unit/integration tests enables fast CI

### CI/CD Pipeline

**GitHub Actions or GitLab CI**

**Pipeline:**
```yaml
build-linux:
  - Build on Ubuntu 22.04
  - Run tests
  - Generate AppImage

build-windows:
  - Build on Windows Server 2022
  - Run tests
  - Generate installer (NSIS)

quality:
  - Run clang-tidy (static analysis)
  - Check code coverage
  - Run sanitizers (ASan, UBSan)
```

**Rationale:**
- ✓ Automated testing on both platforms (NFR-011)
- ✓ Consistent builds
- ✓ Early bug detection

### Development Tools

**Static Analysis:**
- **clang-tidy** - C++ linting
- **cppcheck** - Additional static analysis
- **Qt Creator code model** - Qt-specific checks

**Debugging:**
- **gdb** (Linux) / **MSVC debugger** (Windows)
- **Valgrind** (Linux) - Memory leak detection
- **Dr. Memory** (Windows) - Memory leak detection

**Profiling:**
- **perf** (Linux) / **Visual Studio Profiler** (Windows)
- **Qt Creator profiler** - QML/Qt performance

**Rationale:**
- ✓ Catch bugs early (NFR-007, NFR-012)
- ✓ Ensure performance targets (NFR-001, NFR-002)

---

## System Components

### Component Overview

```
┌─────────────────────────────────────────────────────────────┐
│                  PatternCAD Application                      │
└─────────────────────────────────────────────────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
┌───────▼─────────┐ ┌──────▼───────┐ ┌─────────▼────────┐
│  UI Components  │ │   Command    │ │  Domain Model    │
│  (Qt Widgets)   │ │   System     │ │  (Core Logic)    │
└───────┬─────────┘ └──────┬───────┘ └─────────┬────────┘
        │                   │                   │
        └───────────────────┼───────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
┌───────▼─────────┐ ┌──────▼───────┐ ┌─────────▼────────┐
│  Rendering      │ │  File I/O    │ │  Algorithms      │
│  Engine         │ │  Subsystem   │ │  (Nesting, etc)  │
└─────────────────┘ └──────────────┘ └──────────────────┘
```

### 1. UI Component (Presentation Layer)

**Responsibility:** User interface, event handling, rendering display

**Sub-Components:**

**1.1 MainWindow**
- **Purpose:** Top-level application window
- **Implements:** FR-021 (Multi-Panel Workspace)
- **Classes:**
  - `MainWindow` - QMainWindow subclass
  - `MenuBar` - Application menus
  - `ToolBar` - Quick access tools
  - `StatusBar` - Status information, zoom, coordinates

**1.2 Canvas**
- **Purpose:** Main drawing area
- **Implements:** FR-001, FR-002, FR-003 (Drawing and editing), FR-027 (Isolation mode)
- **Classes:**
  - `PatternCanvas` - QGraphicsView subclass
  - `PatternScene` - QGraphicsScene subclass
  - `IsolationManager` - Manages isolation mode state
- **Features:**
  - Pan (middle-mouse drag, hand tool)
  - Zoom (mouse wheel, pinch gesture)
  - Grid overlay (toggle-able)
  - Rulers (top, left edges)
  - Spacebar grab-and-move interaction (FR-003)

**1.3 Tool Palette**
- **Purpose:** Drawing and editing tool selection
- **Implements:** FR-022 (Keyboard Shortcuts)
- **Classes:**
  - `ToolPalette` - QToolBar or QDockWidget
  - `Tool` (base class) - Abstract tool interface
  - `LineTool`, `CircleTool`, `BezierTool`, etc.
- **Tools:**
  - Select (S)
  - Line (L)
  - Circle (C)
  - Rectangle (R)
  - Arc (A)
  - Bezier (B)
  - Point (P)
  - Move (M)
  - Measure (D)

**1.4 Properties Panel**
- **Purpose:** Display/edit object properties
- **Implements:** FR-021 (Properties Panel)
- **Classes:**
  - `PropertiesPanel` - QDockWidget
  - `PropertyEditor` - QWidget with form layout
- **Properties Shown:**
  - Position (X, Y)
  - Size (Width, Height)
  - Rotation angle
  - Line weight, color, style
  - Constraints, parameters

**1.5 Layers Panel**
- **Purpose:** Layer management
- **Implements:** FR-023 (Layers and Organization)
- **Classes:**
  - `LayersPanel` - QDockWidget
  - `LayerItem` - QTreeWidgetItem subclass
- **Features:**
  - Create/rename/delete layers
  - Toggle visibility (eye icon)
  - Toggle lock (lock icon)
  - Drag-and-drop reordering
  - Layer color indicator

**1.6 Parameters Panel**
- **Purpose:** Parameter management
- **Implements:** FR-004 (Parametric Dimensions)
- **Classes:**
  - `ParametersPanel` - QDockWidget
  - `ParameterEditor` - QTableWidget or custom
- **Features:**
  - Add/edit/delete parameters
  - Parameter groups
  - Formula editor
  - Dependency tracking display

### 2. Command System (Application Layer)

**Responsibility:** Encapsulate all user actions as commands for undo/redo

**Implements:** FR-024 (Undo/Redo System)

**Architecture:**
```cpp
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual QString description() const = 0;
};

class CommandManager {
public:
    void executeCommand(std::unique_ptr<Command> cmd);
    void undo();
    void redo();
    QString undoText() const;
    QString redoText() const;
private:
    std::vector<std::unique_ptr<Command>> undoStack_;
    std::vector<std::unique_ptr<Command>> redoStack_;
};
```

**Command Examples:**
- `CreateLineCommand` - Create new line (FR-001)
- `CreateBezierCommand` - Create bezier curve (FR-002)
- `MoveObjectsCommand` - Move selected objects (FR-003)
- `RotateCommand` - Rotate objects (FR-006)
- `MirrorCommand` - Mirror objects (FR-007)
- `ScaleCommand` - Scale objects (FR-008)
- `AddSeamAllowanceCommand` - Add seam allowance (FR-010)
- `GradePatternCommand` - Create graded sizes (FR-012)
- `AddParameterCommand` - Add parameter (FR-004)
- `AddConstraintCommand` - Add constraint (FR-005)
- `MacroCommand` - Group multiple commands

**Undo/Redo Implementation:**
- Unlimited history (limited by memory ~200MB, NFR-024)
- Delta storage (store changes, not full copies)
- History cleared on file open
- Fast undo/redo (< 50ms, NFR-024)

### 3. Domain Model (Domain Layer)

**Responsibility:** Core business logic, geometry, patterns, constraints

**3.1 Geometry Core**

**Purpose:** Fundamental geometric primitives

**Classes:**
```cpp
class Point2D {
    double x, y;
    Unit unit; // mm, cm, inches
};

class Line {
    Point2D start, end;
};

class Arc {
    Point2D center;
    double radius;
    double startAngle, endAngle;
};

class CubicBezier {
    Point2D p0, p1, p2, p3; // Control points
    // Methods: evaluate(t), length(), split(), offset()
};

class Polyline {
    std::vector<GeometrySegment> segments;
    bool closed;
};

class Shape : public Polyline {
    // Closed polyline representing pattern piece
    BoundingBox boundingBox() const;
    double area() const;
    double perimeter() const;
};
```

**Operations:**
- Intersection, distance, contains point
- Offset (for seam allowance, FR-010)
- Transform (rotate, mirror, scale, FR-006, FR-007, FR-008)
- Arc length parametrization
- Bezier subdivision

**3.2 Pattern Model**

**Purpose:** Represent a pattern piece with geometry, parameters, constraints

**Classes:**
```cpp
class Pattern {
    QString name;
    std::vector<std::shared_ptr<GeometryObject>> geometry;
    std::vector<std::shared_ptr<Parameter>> parameters;
    std::vector<std::shared_ptr<Constraint>> constraints;
    std::vector<std::shared_ptr<Notch>> notches;
    std::shared_ptr<SeamAllowance> seamAllowance;
    Layer* layer;

    // Methods
    BoundingBox boundingBox() const;
    void updateParametric(); // Recompute after parameter change
    void solveConstraints(); // Resolve constraints
};

class Parameter {
    QString name;
    double value;
    Unit unit;
    QString expression; // e.g., "waist_width / 2"
    std::vector<Parameter*> dependencies;
};

class Constraint {
    ConstraintType type; // Distance, Angle, Coincident, etc.
    std::vector<GeometryObject*> objects;
    double value; // For distance, angle constraints

    virtual double evaluate() = 0; // Constraint error
    virtual void solve() = 0; // Adjust geometry to satisfy
};
```

**3.3 Parametric Engine**

**Purpose:** Manage parameters, expressions, dependency graph

**Classes:**
```cpp
class ParametricEngine {
    std::unordered_map<QString, Parameter*> parameters;
    DependencyGraph dependencyGraph;

    void addParameter(Parameter* param);
    void updateParameter(const QString& name, double value);
    void propagateUpdates(); // Update dependent geometry

private:
    std::vector<Parameter*> topologicalSort(); // Resolve update order
    bool detectCircularDependency(); // Prevent infinite loops
};

class ExpressionParser {
    double evaluate(const QString& expression,
                    const std::unordered_map<QString, double>& variables);
};
```

**Update Algorithm:**
1. Parameter value changes
2. Build dependency graph
3. Topological sort (resolve update order)
4. Evaluate expressions in order
5. Update geometry
6. Trigger constraint solving
7. Notify views (observer pattern)

**3.4 Constraint Solver**

**Purpose:** Solve geometric constraints

**Algorithm:** Newton-Raphson with gradient descent fallback

**Classes:**
```cpp
class ConstraintSolver {
    std::vector<Constraint*> constraints;
    std::vector<GeometryObject*> affectedObjects;

    bool solve(int maxIterations = 100);

private:
    Eigen::VectorXd buildErrorVector();
    Eigen::MatrixXd buildJacobian();
    void updateGeometry(const Eigen::VectorXd& delta);
};
```

**Solving Process:**
1. Build error vector (constraint violations)
2. Compute Jacobian (derivatives)
3. Solve linear system: J * Δx = -error
4. Update geometry by Δx
5. Repeat until converged or max iterations
6. If not converged, try gradient descent

**Performance:** Target < 200ms for typical constraint network (NFR-002)

**3.5 Layer System**

**Purpose:** Organize objects into layers

**Implements:** FR-023

**Classes:**
```cpp
class Layer {
    QString name;
    QColor color;
    bool visible;
    bool locked;
    int zOrder;
    std::vector<Pattern*> patterns;
};

class LayerManager {
    std::vector<std::shared_ptr<Layer>> layers;
    Layer* activeLayer;

    Layer* createLayer(const QString& name);
    void deleteLayer(Layer* layer);
    void reorderLayers(Layer* layer, int newIndex);
};
```

**3.6 Project**

**Purpose:** Top-level container for entire project

**Classes:**
```cpp
class Project {
    QString name;
    QString filePath;
    std::vector<std::shared_ptr<Pattern>> patterns;
    std::shared_ptr<LayerManager> layerManager;
    std::shared_ptr<ParametricEngine> parametricEngine;
    ProjectSettings settings; // Units, grid, etc.

    // File operations
    bool save(const QString& path);
    bool load(const QString& path);
    bool exportDXF(const QString& path);
    bool exportSVG(const QString& path);
    bool exportPDF(const QString& path);
};
```

### 4. Rendering Engine

**Responsibility:** Efficient 2D rendering, isolation mode, LOD

**4.1 Scene Graph**

**Architecture:**
```
QGraphicsScene
├─ RootItem
    ├─ LayerItem (for each layer)
    │   └─ PatternItem (for each pattern)
    │       ├─ GeometryItem (lines, curves, arcs)
    │       ├─ NotchItem (notches)
    │       └─ SeamAllowanceItem (seam allowance)
    └─ LayoutSheetItem (layout boundary)
```

**Classes:**
```cpp
class PatternItem : public QGraphicsItemGroup {
    Pattern* pattern_; // Domain model reference

    QRectF boundingRect() const override;
    void paint(QPainter* painter, ...) override;
    QPainterPath shape() const override; // For selection

    void updateFromModel(); // Sync with domain model
};

class GeometryItem : public QAbstractGraphicsShapeItem {
    GeometryObject* geometry_;

    // Render line, arc, bezier curve
    void paint(QPainter* painter, ...) override;
};
```

**4.2 Isolation Mode Manager**

**Purpose:** Manage visibility for isolation mode (FR-027)

**Classes:**
```cpp
class IsolationManager {
    QSet<PatternItem*> isolatedItems;
    bool isolationActive = false;

    void enterIsolation(const QSet<PatternItem*>& items);
    void exitIsolation();
    void addToIsolation(PatternItem* item);
    void removeFromIsolation(PatternItem* item);

private:
    void updateItemVisibility();
};
```

**Implementation:**
- When isolation activated:
  1. Store current selection as isolated set
  2. Hide all items except isolated set
  3. Update scene (triggers partial re-render)
- Performance: < 100ms switch time (NFR-006)

**4.3 Performance Optimizations**

**Spatial Indexing:**
- Qt Graphics View uses BSP tree automatically
- Custom spatial index for very large scenes (> 1000 objects)

**View Frustum Culling:**
- Only render items in visible area
- Automatic in QGraphicsView

**Level of Detail (LOD):**
```cpp
void BezierItem::paint(QPainter* painter, ...) {
    double lod = QStyleOptionGraphicsItem::levelOfDetailFromTransform(
        painter->worldTransform());

    if (lod < 0.1) {
        // Low zoom: render as straight line
        painter->drawLine(start, end);
    } else if (lod < 1.0) {
        // Medium zoom: render with 10 segments
        drawBezierSegments(10);
    } else {
        // High zoom: render with 50 segments
        drawBezierSegments(50);
    }
}
```

**Caching:**
- Use `QGraphicsItem::setCacheMode(DeviceCoordinateCache)` for static items
- Invalidate cache on modification

**OpenGL Acceleration (Optional):**
```cpp
canvas->setViewport(new QOpenGLWidget());
canvas->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
```

### 5. File I/O Subsystem

**Responsibility:** Import/export various file formats

**5.1 Architecture**

**Plugin-Based Design:**
```cpp
class FileFormat {
public:
    virtual ~FileFormat() = default;
    virtual QString name() const = 0;
    virtual QStringList extensions() const = 0;
    virtual bool canImport() const = 0;
    virtual bool canExport() const = 0;

    virtual bool importFile(const QString& path, Project* project) = 0;
    virtual bool exportFile(const QString& path, const Project* project) = 0;
};

class FileFormatManager {
    std::vector<std::unique_ptr<FileFormat>> formats;

    void registerFormat(std::unique_ptr<FileFormat> format);
    FileFormat* getFormat(const QString& extension);
};
```

**5.2 Native Format**

**Purpose:** Save/load complete project state

**Implements:** FR-020

**Format:** JSON

**Structure:**
```json
{
  "version": "1.0",
  "metadata": {
    "created": "2026-01-27T10:00:00Z",
    "modified": "2026-01-27T11:30:00Z",
    "author": "User Name"
  },
  "settings": {
    "units": "mm",
    "grid": {"enabled": true, "spacing": 10}
  },
  "parameters": [
    {"name": "waist_width", "value": 80, "unit": "cm", "expression": ""},
    {"name": "seam", "value": 1, "unit": "cm", "expression": ""}
  ],
  "layers": [
    {"id": "layer1", "name": "Pattern Pieces", "visible": true, "locked": false, "zOrder": 0}
  ],
  "patterns": [
    {
      "id": "pattern1",
      "name": "Front Panel",
      "layer": "layer1",
      "geometry": [
        {"type": "line", "start": [0, 0], "end": [10, 0]},
        {"type": "bezier", "p0": [10, 0], "p1": [12, 2], "p2": [12, 8], "p3": [10, 10]}
      ],
      "constraints": [
        {"type": "distance", "objects": ["point1", "point2"], "value": 10}
      ],
      "seamAllowance": {"width": 1, "corner": "miter"}
    }
  ]
}
```

**Classes:**
```cpp
class NativeFormat : public FileFormat {
    bool importFile(const QString& path, Project* project) override;
    bool exportFile(const QString& path, const Project* project) override;

private:
    QJsonObject serializeProject(const Project* project);
    void deserializeProject(const QJsonObject& json, Project* project);
};
```

**5.3 DXF Format**

**Purpose:** Import/export DXF files

**Implements:** FR-018, FR-019

**Library:** libdxfrw or custom parser (for R12)

**Classes:**
```cpp
class DXFFormat : public FileFormat {
    bool importFile(const QString& path, Project* project) override;
    bool exportFile(const QString& path, const Project* project) override;

private:
    void importEntity(DXFEntity* entity, Pattern* pattern);
    void exportPattern(const Pattern* pattern, DXFWriter* writer);
};
```

**Supported Entities:**
- LINE → Line
- CIRCLE → Circle
- ARC → Arc
- POLYLINE/LWPOLYLINE → Polyline
- SPLINE → Cubic Bezier (approximation)

**5.4 SVG Format**

**Purpose:** Import/export SVG files

**Implements:** FR-018, FR-019

**Classes:**
```cpp
class SVGFormat : public FileFormat {
    bool importFile(const QString& path, Project* project) override;
    bool exportFile(const QString& path, const Project* project) override;

private:
    void parseSVGPath(const QString& pathData, Pattern* pattern);
    QString generateSVGPath(const Polyline* polyline);
};
```

**Supported Elements:**
- path → Polyline with lines/beziers
- line → Line
- circle → Circle
- ellipse → Ellipse
- rect → Rectangle
- polygon → Closed polyline

**5.5 PDF Format**

**Purpose:** Export to PDF for printing/plotting

**Implements:** FR-019

**Classes:**
```cpp
class PDFFormat : public FileFormat {
    bool canImport() const override { return false; }
    bool exportFile(const QString& path, const Project* project) override;

private:
    void renderToPDF(const Project* project, QPdfWriter* writer);
    void handleMultiPageLayout(const Project* project, QPdfWriter* writer);
};
```

**Features:**
- Vector output (no rasterization)
- Multiple page sizes (A4, A3, A2, A1, A0, custom)
- Scale options (1:1, fit to page, custom scale)
- Multi-page support with alignment marks
- Layer selection (include/exclude layers)

**5.6 Auto-Save**

**Purpose:** Automatic background saving for crash recovery

**Implements:** FR-026

**Classes:**
```cpp
class AutoSaveManager : public QObject {
    Q_OBJECT
public:
    void setInterval(int minutes);
    void enable(bool enabled);

private slots:
    void performAutoSave();

private:
    QTimer* timer_;
    Project* project_;
    QString autoSaveDir_;
    int maxVersions_ = 10; // Keep last 10 versions

    void cleanupOldVersions();
};
```

**Implementation:**
- Timer triggers auto-save every N minutes (configurable, default 5 min)
- Save to separate file: `{projectname}-{timestamp}.autosave`
- Run in background thread (non-blocking)
- Keep last 10 versions (FIFO)
- On app restart: check for auto-save files, prompt recovery

### 6. Algorithm Subsystem

**Responsibility:** Computationally intensive algorithms

**6.1 Nesting Algorithm**

**Purpose:** Automatic placement of pattern pieces on material

**Implements:** FR-014

**Classes:**
```cpp
class NestingAlgorithm {
public:
    struct Settings {
        QSizeF materialSize;
        double spacing;
        bool allowRotation;
        std::vector<double> allowedRotations; // e.g., {0, 90, 180, 270}
        std::unordered_map<Pattern*, double> grainDirections;
    };

    struct Result {
        std::vector<PlacedPattern> placements;
        double utilization; // 0.0 to 1.0
        QRectF usedArea;
    };

    Result nest(const std::vector<Pattern*>& patterns, const Settings& settings);
    void cancel(); // Cancel ongoing nesting

signals:
    void progressUpdated(int percent);

private:
    std::atomic<bool> cancelled_{false};
};

struct PlacedPattern {
    Pattern* pattern;
    QPointF position;
    double rotation;
};
```

**Algorithm - Two Phase:**

**Phase 1: Greedy Placement (Fast)**
- Sort pieces by area (largest first)
- For each piece:
  - Try rotations (if allowed)
  - Place at first valid position (bottom-left heuristic)
  - Check spacing constraints
- Time: < 1 second for 50 pieces
- Quality: 60-70% utilization typical

**Phase 2: Genetic Algorithm (Optional, Slower)**
- Use Phase 1 result as initial solution
- Genetic operators: swap, rotate, translate
- Fitness function: minimize bounding box, maximize utilization
- Run for N generations or until no improvement
- Time: 10-30 seconds for 50 pieces
- Quality: 75-85% utilization typical

**Threading:**
- Run in QThread to avoid blocking UI
- Emit progress signals
- Support cancellation (check `cancelled_` flag regularly)

**6.2 Offset Algorithm**

**Purpose:** Generate offset curves for seam allowance

**Implements:** FR-010

**Library:** Clipper or Boost.Geometry

**Classes:**
```cpp
class OffsetAlgorithm {
public:
    enum CornerType { Miter, Round, Bevel };

    Polyline offset(const Polyline& input, double distance, CornerType corner);

private:
    // Use Clipper or Boost.Geometry offset
};
```

**6.3 Grading Algorithm**

**Purpose:** Generate multiple sizes from grading rules

**Implements:** FR-012

**Classes:**
```cpp
class GradingAlgorithm {
public:
    struct GradeRule {
        QString dimension; // e.g., "width", "length"
        std::vector<double> increments; // Change per size
    };

    std::vector<Pattern*> grade(const Pattern* basePattern,
                                 const std::vector<GradeRule>& rules,
                                 const QStringList& sizeNames);
};
```

**Implementation:**
- For each size:
  - Clone base pattern
  - Apply parameter adjustments
  - Recompute parametric system
  - Resolve constraints

---

## Data Architecture

### Data Model

**Core Entities:**

```
Project
├─ Settings (units, grid, etc.)
├─ Parameters (global parameters)
├─ LayerManager
│   └─ Layers (collection)
└─ Patterns (collection)
    ├─ Geometry (lines, curves, shapes)
    ├─ Parameters (pattern-specific)
    ├─ Constraints
    ├─ Notches
    ├─ SeamAllowance
    └─ Metadata (name, layer, etc.)
```

**Entity Relationships:**

```
Project 1───* Pattern
Pattern *───1 Layer
Pattern 1───* GeometryObject
Pattern 1───* Parameter
Pattern 1───* Constraint
Parameter *───* Parameter (dependencies)
Constraint *───* GeometryObject
```

### Data Flow

**Creating a Pattern Piece:**

```
User Input (Canvas)
    │
    ├─> Tool (e.g., LineTool)
    │       │
    │       └─> CreateLineCommand
    │               │
    │               └─> Pattern::addGeometry(Line*)
    │                       │
    │                       └─> GeometryObject stored
    │                           │
    │                           └─> Observer notification
    │                                   │
    │                                   └─> PatternItem::updateFromModel()
    │                                           │
    │                                           └─> Canvas renders update
```

**Parametric Update:**

```
User Changes Parameter Value
    │
    └─> ParametricEngine::updateParameter()
            │
            ├─> Rebuild dependency graph
            ├─> Topological sort
            └─> For each dependent parameter:
                    │
                    ├─> Evaluate expression
                    ├─> Update parameter value
                    └─> Notify dependent geometry
                            │
                            └─> Geometry::updateFromParameters()
                                    │
                                    ├─> Recompute positions
                                    └─> Trigger constraint solving
                                            │
                                            └─> ConstraintSolver::solve()
                                                    │
                                                    └─> Adjust geometry to satisfy constraints
                                                            │
                                                            └─> Observer notification
                                                                    │
                                                                    └─> Canvas re-renders
```

**File Save:**

```
User: File > Save
    │
    └─> Project::save(path)
            │
            ├─> Serialize to JSON
            │       │
            │       ├─> Serialize settings
            │       ├─> Serialize parameters
            │       ├─> Serialize layers
            │       └─> Serialize patterns
            │               │
            │               ├─> Serialize geometry
            │               ├─> Serialize constraints
            │               └─> Serialize metadata
            │
            └─> Write to file atomically
                    │
                    ├─> Write to temp file
                    ├─> Sync to disk
                    └─> Rename (atomic)
```

---

## Non-Functional Requirements Coverage

### NFR-001: Performance - Rendering and Canvas Response

**Requirement:** 60fps rendering, < 100ms zoom, < 50ms pan/selection

**Architecture Solutions:**

1. **Qt Graphics View Framework**
   - Hardware-accelerated rendering (optional OpenGL)
   - Built-in view frustum culling
   - Spatial indexing (BSP tree)

2. **Efficient Scene Graph**
   - QGraphicsItem caching for static items
   - Level of Detail (LOD) for curves at low zoom
   - Lazy evaluation of bounding boxes

3. **Optimized Event Handling**
   - Event coalescing for mouse move
   - Direct connection for critical signals
   - Minimal processing in event handlers

4. **Profiling and Optimization**
   - Profile with Qt Creator profiler
   - Optimize hot paths identified by profiling
   - Target: 60fps maintained for 200 objects

**Verification:**
- Performance tests with 200 objects
- Measure frame time during pan/zoom
- Ensure < 16ms frame time (60fps)

---

### NFR-002: Performance - Complex Pattern Handling

**Requirement:** 500+ objects responsive, 1000+ objects usable (30fps)

**Architecture Solutions:**

1. **Isolation Mode (FR-027)**
   - Render only isolated pieces
   - Dramatically reduces rendering load
   - 5 pieces isolated from 500 = 5-piece performance

2. **Spatial Indexing**
   - BSP tree or R-tree for fast lookup
   - Only check nearby items for intersection/selection

3. **Lazy Evaluation**
   - Compute bounding boxes on demand
   - Cache computed values until geometry changes

4. **Memory Management**
   - Shared pointers for geometry reuse
   - Copy-on-write for transformations
   - Pool allocator for small objects

5. **Constraint Solver Optimization**
   - Sparse matrix representation
   - Iterative solving (stop when converged)
   - Target: < 200ms for typical constraint network

**Verification:**
- Load tests with 500, 1000, 2000 objects
- Measure FPS, memory usage
- Ensure 30fps minimum for 1000 objects

---

### NFR-003: Security - File and Data Protection

**Requirement:** Protect user data, atomic saves, proper permissions

**Architecture Solutions:**

1. **Atomic File Saves**
   ```cpp
   bool Project::save(const QString& path) {
       QString tempPath = path + ".tmp";

       // Write to temp file
       if (!writeToFile(tempPath)) return false;

       // Sync to disk
       syncToDisk(tempPath);

       // Atomic rename (overwrites original)
       if (!QFile::rename(tempPath, path)) {
           QFile::remove(tempPath);
           return false;
       }

       return true;
   }
   ```

2. **Auto-Save Protection**
   - Auto-save files in separate directory
   - Proper file permissions (0600 on Linux)
   - Recovery on crash

3. **No Telemetry**
   - All data stays local
   - No network connections (except optional update check)
   - No analytics or usage tracking

**Verification:**
- Test file save during crash simulation
- Verify file permissions on Linux
- Code review for data leaks

---

### NFR-004: Security - Input Validation

**Requirement:** Validate all inputs, handle malformed files gracefully

**Architecture Solutions:**

1. **Numeric Input Validation**
   ```cpp
   class NumericInput : public QLineEdit {
       void focusOutEvent(QFocusEvent* e) override {
           double value = text().toDouble();
           if (value < min_ || value > max_) {
               // Show error, reset to previous value
               setText(QString::number(previousValue_));
           }
       }
   };
   ```

2. **File Import Validation**
   ```cpp
   bool DXFFormat::importFile(const QString& path, Project* project) {
       try {
           DXFReader reader(path);

           // Validate version
           if (!isSupportedVersion(reader.version())) {
               showError("Unsupported DXF version");
               return false;
           }

           // Parse entities with error handling
           for (auto* entity : reader.entities()) {
               try {
                   importEntity(entity, project);
               } catch (const std::exception& e) {
                   logWarning("Failed to import entity: " + e.what());
                   // Continue with other entities
               }
           }

           return true;

       } catch (const std::exception& e) {
           showError("Failed to open DXF file: " + e.what());
           return false;
       }
   }
   ```

3. **Parameter Expression Validation**
   - Parse expressions before evaluation
   - Detect division by zero
   - Limit recursion depth
   - Reject invalid variable names

**Verification:**
- Fuzzing tests with malformed files
- Unit tests for input validation
- Manual testing with invalid inputs

---

### NFR-005: Scalability - Project Size Limits

**Requirement:** Support 2000 objects, 100 layers, 200 parameters

**Architecture Solutions:**

1. **Efficient Data Structures**
   - `std::vector` for sequences (compact memory)
   - `std::unordered_map` for lookups (O(1) average)
   - Shared pointers to avoid copies

2. **Memory Budget Tracking**
   ```cpp
   class MemoryTracker {
       size_t estimateProjectSize(const Project* project);
       void warnIfExceeding(size_t limit);
   };
   ```

3. **Limits Enforcement**
   ```cpp
   static constexpr size_t MAX_OBJECTS = 2000;
   static constexpr size_t MAX_LAYERS = 100;
   static constexpr size_t MAX_PARAMETERS = 200;

   bool Project::addPattern(Pattern* pattern) {
       if (patterns_.size() >= MAX_OBJECTS) {
           showError("Maximum object count reached");
           return false;
       }
       patterns_.push_back(pattern);
       return true;
   }
   ```

**Verification:**
- Create projects at limit boundaries
- Measure memory usage
- Ensure graceful degradation

---

### NFR-006: Scalability - Multi-Pattern Projects (300-500 pieces)

**Requirement:** 300 pieces good performance, 500 pieces acceptable with isolation

**Architecture Solutions:**

1. **Isolation Mode (FR-027)**
   - **Key enabler for 500 piece support**
   - Render only isolated pieces
   - Switch time < 100ms

2. **Lazy Loading**
   - Don't create QGraphicsItems until needed
   - Unload items far from view

3. **Scene Partitioning**
   - Divide scene into regions
   - Only process items in active region

4. **Efficient Picking**
   - BSP tree for fast item lookup
   - Bounding box tests before shape tests

**Performance Targets:**
- 300 pieces: 60fps full view
- 500 pieces: 30fps full view, 60fps isolation mode

**Verification:**
- Load tests with 300, 500 pieces
- Measure FPS in various modes
- User acceptance testing

---

### NFR-007: Reliability - Crash Prevention

**Requirement:** < 1 crash per 100 hours, graceful error handling

**Architecture Solutions:**

1. **Defensive Programming**
   - Null pointer checks
   - Bounds checking
   - Assert in debug builds

2. **Exception Handling**
   ```cpp
   void MainWindow::handleCommand(Command* cmd) {
       try {
           commandManager_->executeCommand(cmd);
       } catch (const std::exception& e) {
           showError("Operation failed: " + e.what());
           // Log error, don't crash
       }
   }
   ```

3. **Resource Management**
   - RAII for all resources
   - Smart pointers (no manual delete)
   - Qt parent-child ownership

4. **Memory Safety**
   - AddressSanitizer in CI
   - No raw pointers for ownership
   - Prefer references over pointers

5. **Circular Dependency Detection**
   ```cpp
   bool ParametricEngine::detectCircularDependency() {
       // Depth-first search for cycles in dependency graph
       // Return true if cycle detected
   }
   ```

**Verification:**
- Automated testing (unit, integration)
- Fuzzing tests
- Manual QA testing
- Beta testing with target users

---

### NFR-008: Reliability - Error Recovery

**Requirement:** Clear error messages, transaction-like operations

**Architecture Solutions:**

1. **User-Friendly Error Messages**
   ```cpp
   void showError(const QString& operation, const QString& reason,
                  const QString& suggestion) {
       QString message = QString("Failed to %1.\n\n"
                                 "Reason: %2\n\n"
                                 "Suggestion: %3")
                         .arg(operation, reason, suggestion);
       QMessageBox::critical(mainWindow, "Error", message);
   }

   // Example usage:
   showError("save file",
             "Disk is full",
             "Free up disk space and try again.");
   ```

2. **Command Pattern for Transactions**
   - All operations encapsulated as commands
   - If command fails, undo is called automatically
   - Document state never left inconsistent

3. **Auto-Save Recovery**
   - On crash, auto-save files preserved
   - On restart, offer recovery
   - User can preview recovery file before opening

4. **Logging**
   ```cpp
   class Logger {
       void logInfo(const QString& message);
       void logWarning(const QString& message);
       void logError(const QString& message);

       // Log to file: ~/.local/share/PatternCAD/logs/
   };
   ```

**Verification:**
- Test error paths
- Simulate failures (disk full, permissions, etc.)
- Verify error messages are actionable

---

### NFR-009: Usability - User Interface Clarity

**Requirement:** New users productive in 30 min, clear tooltips, visual feedback

**Architecture Solutions:**

1. **Consistent UI Design**
   - Follow platform guidelines (Qt style)
   - Consistent icon set
   - Standard keyboard shortcuts

2. **Tooltips Everywhere**
   ```cpp
   tool->setToolTip("Line Tool (L)\nClick to place start point, "
                    "click again for end point.");
   ```

3. **Visual Feedback**
   - Hover highlights
   - Selection indicators (bounding box, handles)
   - Active tool indicator
   - Status bar messages

4. **Descriptive Undo/Redo**
   ```cpp
   QString MoveObjectsCommand::description() const {
       return QString("Move %1 object(s)").arg(objects_.size());
   }
   ```

5. **Inline Help**
   - Status bar shows tool instructions
   - Properties panel has "?" button for help
   - First-run tutorial (optional)

**Verification:**
- Usability testing with target users
- Measure time to complete first pattern
- Collect feedback on UI clarity

---

### NFR-010: Usability - Accessibility Standards

**Requirement:** High contrast mode, keyboard navigation, scalable fonts

**Architecture Solutions:**

1. **High Contrast Mode**
   - Qt stylesheet for high contrast theme
   - User toggle in preferences

2. **Keyboard Navigation**
   - Tab order through all controls
   - Keyboard shortcuts for all major functions
   - Arrow keys for navigation

3. **Scalable UI**
   - Use layout managers (not fixed sizes)
   - Fonts specified in points (not pixels)
   - Respect system DPI settings
   - Test on 4K displays

4. **Color Not Sole Indicator**
   - Use icons + color
   - Patterns in addition to colors
   - Text labels where appropriate

**Verification:**
- Test with screen reader (Orca on Linux, NVDA on Windows)
- Test keyboard-only navigation
- Test on high-DPI displays

---

### NFR-011: Usability - Cross-Platform Consistency

**Requirement:** Identical UI and behavior on Linux and Windows

**Architecture Solutions:**

1. **Single Codebase**
   - Qt framework abstracts platform differences
   - No platform-specific UI code (except where necessary)

2. **Consistent Behavior**
   - Same file format on both platforms
   - Same results for all operations
   - Same keyboard shortcuts (with Ctrl on both)

3. **Platform-Specific Adaptations**
   - File dialogs use native OS dialogs
   - Menu bar follows platform convention
   - Window decorations match OS theme

4. **Testing on Both Platforms**
   - CI builds and tests on Linux and Windows
   - Manual testing on both before release

**Verification:**
- Open same file on Linux and Windows, verify identical results
- Test file exchange between platforms
- User testing on both platforms

---

### NFR-012: Maintainability - Code Quality

**Requirement:** Clean code, consistent style, modular design

**Architecture Solutions:**

1. **Layered Architecture**
   - Clear separation: UI, Application, Domain, Infrastructure
   - Dependencies flow downward only

2. **Code Style Enforcement**
   - clang-format configuration
   - clang-tidy checks
   - Code review required

3. **Single Responsibility**
   - Classes have one clear purpose
   - Functions < 50 lines (guideline)
   - Modules have cohesive functionality

4. **Documentation**
   ```cpp
   /**
    * @brief Solve geometric constraints using Newton-Raphson method.
    *
    * Iteratively adjusts geometry to minimize constraint violations.
    * Falls back to gradient descent if Newton-Raphson doesn't converge.
    *
    * @param maxIterations Maximum number of iterations (default: 100)
    * @return true if converged, false if max iterations reached
    */
   bool ConstraintSolver::solve(int maxIterations = 100);
   ```

5. **Dependency Injection**
   - Constructor injection for dependencies
   - Easier testing and mocking

**Verification:**
- Code reviews
- Static analysis (clang-tidy)
- Contributor onboarding (measure time to productivity)

---

### NFR-013: Maintainability - Testing Coverage

**Requirement:** 60% coverage core modules, 40% overall

**Architecture Solutions:**

1. **Unit Tests (gtest)**
   ```cpp
   TEST(BezierTest, EvaluateAtT) {
       CubicBezier bezier(Point2D(0,0), Point2D(1,1),
                          Point2D(2,1), Point2D(3,0));
       Point2D p = bezier.evaluate(0.5);
       EXPECT_NEAR(p.x, 1.5, 0.01);
       EXPECT_NEAR(p.y, 0.75, 0.01);
   }

   TEST(ConstraintSolverTest, DistanceConstraint) {
       // Test constraint solving
   }
   ```

2. **Integration Tests**
   - Test file import/export
   - Test command execution and undo
   - Test parametric updates

3. **UI Tests (Qt Test)**
   ```cpp
   void CanvasTest::testSpacebarGrabMove() {
       // Simulate spacebar grab-and-move interaction
       QTest::mouseClick(canvas, Qt::LeftButton, Qt::NoModifier, QPoint(100, 100));
       QTest::keyPress(canvas, Qt::Key_Space);
       QTest::mouseMove(canvas, QPoint(150, 150));
       QTest::keyPress(canvas, Qt::Key_Space);

       // Verify object moved
       QCOMPARE(object->pos(), QPointF(150, 150));
   }
   ```

4. **CI Integration**
   - Run tests on every commit
   - Block merge if tests fail
   - Coverage report generation

**Verification:**
- Measure coverage with lcov/gcov
- Track coverage over time
- Review test quality (not just coverage %)

---

### NFR-014: Maintainability - Documentation

**Requirement:** User guide, quick start, developer docs, API docs

**Architecture Solutions:**

1. **User Documentation**
   - Markdown-based user guide in `docs/user/`
   - Quick start tutorial (15 minutes)
   - Screenshots and examples
   - FAQ section

2. **Developer Documentation**
   - Architecture overview (this document)
   - Build instructions in `docs/developer/BUILD.md`
   - Contribution guide in `CONTRIBUTING.md`
   - Code style guide

3. **API Documentation**
   - Doxygen for C++ code
   - Generated from source comments
   - Published to GitHub Pages

4. **Inline Documentation**
   - Comments for complex algorithms
   - Class/function docstrings
   - README in each module directory

**Structure:**
```
docs/
├── user/
│   ├── getting-started.md
│   ├── drawing-tools.md
│   ├── parametric-design.md
│   ├── file-formats.md
│   └── keyboard-shortcuts.md
├── developer/
│   ├── architecture.md (this document)
│   ├── BUILD.md
│   ├── TESTING.md
│   └── CONTRIBUTING.md
└── api/
    └── (generated by Doxygen)
```

**Verification:**
- Documentation review
- User testing with docs
- Developer onboarding feedback

---

### NFR-015: Compatibility - File Format Standards

**Requirement:** DXF, SVG, PDF compatible with industry tools

**Architecture Solutions:**

1. **Standards Compliance**
   - DXF: Support R12 (universal compatibility)
   - SVG: Follow SVG 1.1 spec
   - PDF: Use PDF/A for archival

2. **Compatibility Testing**
   - Test matrix:
     - Export DXF → Open in AutoCAD, QCAD, LibreCAD
     - Export SVG → Open in Inkscape, Illustrator
     - Export PDF → Open in Adobe Reader, plotters

3. **Metadata Inclusion**
   ```xml
   <!-- SVG metadata -->
   <metadata>
     <creator>PatternCAD v1.0</creator>
     <created>2026-01-27</created>
     <units>mm</units>
     <scale>1:1</scale>
   </metadata>
   ```

4. **Format Documentation**
   - Document native format specification
   - Versioning strategy
   - Migration path for breaking changes

**Verification:**
- Automated compatibility tests
- Manual testing with industry tools
- User feedback on interoperability

---

### NFR-016: Compatibility - Hardware Requirements

**Requirement:** Run on modest hardware (2018+ era, 4GB RAM)

**Architecture Solutions:**

1. **Minimal Dependencies**
   - Qt framework (cross-platform)
   - No heavy external dependencies

2. **Efficient Memory Usage**
   - Target: < 500MB for typical project (100 objects)
   - Monitor memory with profiler
   - Optimize data structures

3. **Software Rendering Fallback**
   - OpenGL acceleration optional
   - Qt software renderer always available
   - Detect GPU capabilities at startup

4. **Performance Scaling**
   - Isolation mode for large projects
   - LOD adjusts to hardware capability

**Target Hardware:**
- CPU: Dual-core 2 GHz (2018 era)
- RAM: 4 GB
- GPU: Integrated graphics (Intel HD 4000+)
- Disk: 200 MB + project files

**Verification:**
- Test on minimum spec hardware
- Measure memory and CPU usage
- Ensure acceptable performance

---

### NFR-017: Compatibility - Operating System Versions

**Requirement:** Support maintained OS versions (Ubuntu 20.04+, Windows 10+, Manjaro)

**Architecture Solutions:**

1. **Qt Compatibility**
   - Qt 6.5+ supports:
     - Ubuntu 20.04+ (glibc 2.31+)
     - Windows 10 version 1809+
     - Manjaro current and previous

2. **Build for Compatibility**
   - Linux: Build on oldest supported distro (Ubuntu 20.04)
   - Windows: Build with Windows 10 SDK

3. **Distribution**
   - **Linux:**
     - AppImage (universal, no dependencies)
     - .deb package (Debian/Ubuntu)
     - .rpm package (Fedora/Red Hat)
     - Flatpak (optional, Flathub)
   - **Windows:**
     - Installer (.exe with NSIS or Inno Setup)
     - Portable .zip

4. **Version Detection**
   ```cpp
   void checkOSCompatibility() {
       if (QSysInfo::productType() == "windows") {
           // Check Windows version
       } else if (QSysInfo::productType() == "ubuntu") {
           // Check Ubuntu version
       }
       // Warn if unsupported
   }
   ```

**Verification:**
- Test on all supported OS versions
- Document supported versions clearly
- Provide warnings for unsupported OS

---

## Deployment Architecture

### Environments

**Development:**
- Developer workstations (Linux or Windows)
- Local builds and testing

**CI/CD:**
- GitHub Actions or GitLab CI
- Automated builds and tests on Linux and Windows
- Artifact generation (AppImage, .deb, .rpm, Windows installer)

**Release:**
- GitHub Releases for distribution
- Checksums and GPG signatures for verification
- Release notes and changelog

### Build and Packaging

**Linux Packaging:**

**1. AppImage (Primary)**
```bash
# Build AppImage using linuxdeploy
linuxdeploy --appdir=AppDir --executable=patterncad \
            --desktop-file=patterncad.desktop \
            --icon-file=patterncad.png \
            --plugin qt --output appimage
```
- Advantages: Universal, no installation, portable
- Distribution: Single file download

**2. .deb Package**
```bash
# Using dpkg-deb
dpkg-deb --build patterncad_1.0.0_amd64
```
- For Debian/Ubuntu users
- Integrates with system package manager

**3. .rpm Package**
```bash
# Using rpmbuild
rpmbuild -ba patterncad.spec
```
- For Fedora/Red Hat users

**4. Flatpak (Future)**
- Distribution via Flathub
- Sandboxed environment

**Windows Packaging:**

**1. NSIS Installer**
```nsis
; PatternCAD installer script
!define APP_NAME "PatternCAD"
!define APP_VERSION "1.0.0"

OutFile "PatternCAD-${APP_VERSION}-Setup.exe"
InstallDir "$PROGRAMFILES64\PatternCAD"

Section "Install"
  SetOutPath $INSTDIR
  File /r "bin\*"
  File /r "lib\*"

  ; Create shortcuts
  CreateShortCut "$DESKTOP\PatternCAD.lnk" "$INSTDIR\patterncad.exe"
  CreateShortCut "$SMPROGRAMS\PatternCAD.lnk" "$INSTDIR\patterncad.exe"

  ; Register file associations
  WriteRegStr HKCR ".patterncad" "" "PatternCAD.Document"
  WriteRegStr HKCR "PatternCAD.Document\DefaultIcon" "" "$INSTDIR\patterncad.exe,0"
  WriteRegStr HKCR "PatternCAD.Document\shell\open\command" "" '"$INSTDIR\patterncad.exe" "%1"'
SectionEnd
```

**2. Portable .zip**
- No installation required
- Extract and run
- For users who prefer portable apps

### Deployment Strategy

**Release Process:**

1. **Version Tagging**
   ```bash
   git tag -a v1.0.0 -m "Release version 1.0.0"
   git push origin v1.0.0
   ```

2. **Automated Build**
   - CI/CD pipeline triggered by tag
   - Build on Linux (Ubuntu 22.04)
   - Build on Windows (Server 2022)
   - Run all tests

3. **Package Generation**
   - Create AppImage, .deb, .rpm (Linux)
   - Create installer, portable zip (Windows)
   - Generate checksums (SHA256)
   - Sign with GPG key

4. **Release Publication**
   - Create GitHub Release
   - Upload artifacts
   - Attach checksums and signatures
   - Write release notes

5. **Distribution**
   - GitHub Releases (primary)
   - Project website (future)
   - Flathub (future, for Flatpak)

**Update Mechanism:**

- **Manual Updates (v1.0):**
  - User checks for updates manually
  - Download from GitHub Releases

- **Future: Auto-Update Check**
  - Check for updates on startup (optional)
  - Notify user if new version available
  - Link to download page

---

## Trade-offs & Decision Log

### Decision 1: Qt vs. Electron

**Options:**
- Qt (C++)
- Electron (JavaScript/TypeScript)

**Decision:** Qt

**Rationale:**
- ✓ **Performance:** Native C++ performance critical for 300+ pieces and 60fps rendering
- ✓ **Memory:** Qt much lighter than Electron (Chromium)
- ✓ **Cross-platform:** Both support Linux/Windows, but Qt has better native feel
- ✓ **Graphics:** Qt Graphics View excellent for 2D CAD applications
- ✗ **Learning curve:** Qt C++ steeper than Electron web tech
- ✗ **Development speed:** Electron potentially faster for web developers

**Consequences:**
- Need C++ expertise
- Excellent performance
- Smaller binary size
- Native look and feel

---

### Decision 2: Custom Constraint Solver vs. Existing Library

**Options:**
- Custom implementation
- SolveSpace solver (GPL)
- FreeCAD Sketcher (LGPL)

**Decision:** Custom implementation

**Rationale:**
- ✓ **License:** Full control, LGPL/GPL compatible
- ✓ **Simplicity:** Can start simple, add complexity as needed
- ✓ **Learning:** Team learns constraint solving
- ✗ **Effort:** More initial development
- ✗ **Maturity:** Won't be as robust initially

**Mitigation:**
- Start with basic constraints (distance, angle, parallel, perpendicular)
- Iterate based on user feedback
- Can integrate existing solver later if needed

---

### Decision 3: JSON vs. Binary Native Format

**Options:**
- JSON (text-based)
- Binary format (custom or ProtoBuf)

**Decision:** JSON

**Rationale:**
- ✓ **Readability:** Human-readable, debuggable
- ✓ **Tools:** Easy to parse, edit, diff
- ✓ **Simplicity:** Qt JSON module built-in
- ✗ **Size:** Larger file size than binary
- ✗ **Speed:** Slower parse than binary

**Mitigation:**
- Acceptable for pattern files (typically < 10MB)
- Can compress (.patterncad.gz) if size becomes issue
- Can add binary format later for large files

---

### Decision 4: Greedy vs. Optimal Nesting

**Options:**
- Greedy heuristic only
- Optimal algorithm (genetic, simulated annealing)
- Two-phase approach (greedy then optimal)

**Decision:** Two-phase approach

**Rationale:**
- ✓ **Fast feedback:** Greedy gives quick result (< 1 sec)
- ✓ **Better result:** Optional optimization for better utilization
- ✓ **User control:** User can stop optimization when satisfied
- ✗ **Complexity:** More code to maintain

**Implementation:**
- Phase 1 (always): Greedy, fast (< 1 sec for 50 pieces)
- Phase 2 (optional): Genetic algorithm (10-30 sec for 50 pieces)
- User can cancel Phase 2 and use Phase 1 result

---

### Decision 5: AppImage vs. .deb as Primary Linux Distribution

**Options:**
- AppImage (universal binary)
- .deb package
- Flatpak
- Snap

**Decision:** AppImage primary, .deb secondary

**Rationale:**
- ✓ **Universal:** Works on all distros without modification
- ✓ **No installation:** Can run directly
- ✓ **Portable:** Single file, can run from USB
- ✓ **Familiar:** Established format with good tooling
- ✗ **Integration:** Less system integration than .deb

**Mitigation:**
- Provide .deb for Ubuntu/Debian users who prefer package manager
- AppImage is primary for maximum compatibility

---

## Open Issues & Risks

### Technical Risks

**Risk 1: Performance with 500+ Pieces**

**Description:** May not achieve 60fps with 500 pieces in full view mode

**Mitigation:**
- Isolation mode (FR-027) provides escape hatch
- Target 30fps acceptable for full view of 500 pieces
- LOD and culling optimizations
- Profile and optimize hot paths

**Status:** Medium risk, mitigation in place

---

**Risk 2: Constraint Solver Convergence**

**Description:** Custom constraint solver may not converge for complex constraint networks

**Mitigation:**
- Start with simple constraints
- Implement gradient descent fallback
- Warn user if not converged
- Can integrate mature solver (SolveSpace) if needed

**Status:** Medium risk, fallback plan available

---

**Risk 3: DXF/SVG Compatibility**

**Description:** Exported files may not open correctly in all tools due to format quirks

**Mitigation:**
- Target lowest common denominator (DXF R12, SVG 1.1)
- Extensive compatibility testing
- Document known issues
- Provide feedback mechanism for users to report compatibility problems

**Status:** Low risk, well-defined formats

---

### Resource Risks

**Risk 4: C++ Development Expertise**

**Description:** Team may need ramp-up time for Qt C++ development

**Mitigation:**
- Qt documentation is excellent
- Large community and examples
- Can hire contractors if needed
- Phased development allows learning

**Status:** Low risk for experienced C++ developers

---

**Risk 5: Testing Coverage**

**Description:** May not achieve 60% test coverage target

**Mitigation:**
- Prioritize core modules (geometry, parametric, constraints)
- UI testing optional
- Focus on critical paths

**Status:** Low risk, achievable with discipline

---

### External Dependencies

**Risk 6: Qt Licensing Changes**

**Description:** Qt could change licensing in future versions

**Mitigation:**
- Qt 6.5 LTS available under LGPL
- Can stay on LTS version if needed
- Open-source community maintains LGPL fork if necessary

**Status:** Very low risk, Qt committed to open source

---

**Risk 7: Third-Party Library Availability**

**Description:** Libraries like libdxfrw (GPL) may have licensing conflicts

**Mitigation:**
- Dynamic linking allowed for GPL libraries
- Can implement custom parsers for critical formats (DXF R12)
- Alternative libraries available

**Status:** Low risk, workarounds available

---

## Assumptions & Constraints

### Assumptions

1. **Target users have modest hardware (2018+ era)**
   - Dual-core CPU, 4GB RAM, integrated graphics
   - Validation: Survey target user community

2. **Users willing to learn CAD-style interface**
   - Not expecting "point and click" simplicity
   - Validation: User testing, tutorials

3. **Standard file formats (DXF, SVG) provide adequate interoperability**
   - Don't need to support proprietary formats (e.g., Gerber, Illustrator)
   - Validation: User feedback, compatibility testing

4. **Open-source community will contribute over time**
   - Need contributions for sustainability
   - Validation: Make project welcoming, good documentation

5. **Qt 6 will remain stable and supported**
   - Qt 6.5 LTS available until 2026+
   - Validation: Qt roadmap

### Constraints

1. **Must support Linux and Windows with single codebase**
   - No platform-specific UI code (except where necessary)
   - Drives: Framework choice (Qt)

2. **Must be open-source (LGPL or MIT license)**
   - No GPL libraries for core (dynamic linking OK)
   - Drives: Library selection

3. **Limited development resources initially**
   - Small team or solo developer
   - Drives: Phased development, prioritization

4. **No cloud infrastructure for v1.0**
   - Desktop-only, no server component
   - Drives: Architecture (no backend)

5. **Must run without installation (AppImage)**
   - Portable, no admin rights required
   - Drives: Packaging strategy

---

## Future Considerations

### Post-1.0 Features

**macOS Support:**
- Qt supports macOS
- Would require macOS build machine for CI
- User demand to be evaluated

**Template Library:**
- Community-contributed pattern templates
- Local library + optional online sync
- Requires moderation, hosting infrastructure

**3D Visualization:**
- Preview pattern pieces in 3D (draped on form)
- Not full simulation, just visualization
- Would require 3D engine (Qt3D or custom OpenGL)

**Cloud Sync (Optional):**
- Sync projects across devices
- Requires backend infrastructure
- Privacy-focused (end-to-end encryption)

**Collaboration Features:**
- Real-time collaboration (like Google Docs)
- Requires complex CRDT or operational transforms
- Requires backend infrastructure

**Mobile Companion App:**
- View patterns on tablet/phone
- Limited editing (annotations, measurements)
- Requires mobile development (Qt Quick or native)

**Plugin System:**
- User-created plugins for custom tools
- Scripting API (Python bindings via PySide6)
- Security considerations

**Advanced Nesting:**
- Support for irregular material shapes
- Multi-material optimization
- Integration with cutting machines (direct control)

### Technical Debt

**Items to Revisit:**

1. **Custom Constraint Solver:**
   - If custom solver insufficient, integrate SolveSpace
   - Evaluate after v1.0 user feedback

2. **OpenGL Rendering:**
   - If software rendering insufficient, enable OpenGL by default
   - Requires compatibility testing

3. **DXF Library Licensing:**
   - If GPL libraries problematic, implement custom DXF parser
   - DXF R12 format is simple enough

4. **Memory Optimization:**
   - If 2000 object limit too low, implement paging/streaming
   - Unlikely to be needed for v1.0

---

## Approval & Sign-off

**Review Status:**
- [ ] Technical Lead / Architect
- [ ] Core Developer(s)
- [ ] Community Representatives
- [ ] Documentation Lead

**Approval Criteria:**
- Architecture addresses all FRs and NFRs from PRD
- Technology choices justified and appropriate
- Risks identified and mitigated
- Path to implementation clear

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-01-27 | System Architect | Initial architecture document |

---

## Next Steps

### Phase 4: Sprint Planning & Implementation

Run `/sprint-planning` to:
- Break epics into detailed user stories
- Estimate story complexity (story points)
- Plan sprint iterations
- Begin implementation following this architectural blueprint

**Implementation Priority:**

**Sprint 0: Foundation (2-3 weeks)**
- Set up build system (CMake, vcpkg/Conan)
- Set up CI/CD pipeline (GitHub Actions)
- Create project skeleton with Qt
- Implement basic geometry classes
- Set up testing framework

**Sprint 1-2: Core Drawing (3-4 weeks)**
- EPIC-008: UI Foundation (main window, canvas, panels)
- EPIC-001: Core Drawing Tools (line, circle, bezier, select, move)
- Command system and undo/redo

**Sprint 3-4: Transformations (2-3 weeks)**
- EPIC-003: Transformations (rotate, mirror, scale, align)

**Sprint 5-7: Parametric System (4-5 weeks)**
- EPIC-002: Parametric design and constraints
- Constraint solver implementation

**Sprint 8-10: Pattern Features (4-5 weeks)**
- EPIC-004: Professional pattern features (seam allowance, notches, grading)

**Sprint 11-12: Layout and Isolation (3-4 weeks)**
- EPIC-005: Layout, nesting, isolation mode

**Sprint 13-14: Measurement (2-3 weeks)**
- EPIC-006: Measurement and dimensioning

**Sprint 15-18: File I/O (5-6 weeks)**
- EPIC-007: File operations (native, DXF, SVG, PDF, auto-save)

**Sprint 19-20: Polish and Testing (3-4 weeks)**
- Bug fixes
- Performance optimization
- Documentation
- User testing

**Total Estimated Time: 30-40 weeks (7-9 months)**

**Key Implementation Principles:**

1. **Follow layered architecture** - Keep UI, application, domain, infrastructure separate
2. **Command pattern for all operations** - Ensures undo/redo works
3. **Test as you go** - Unit tests for domain logic, integration tests for workflows
4. **Profile early** - Identify performance bottlenecks early
5. **Cross-platform testing** - Test on Linux and Windows throughout

---

**This document was created using BMAD Method v6 - Phase 3 (Solutioning)**

*To continue: Run `/workflow-status` to see your progress and next recommended workflow.*

---

## Appendix A: Technology Evaluation Matrix

| Technology | Pros | Cons | Score (1-5) | Selected |
|------------|------|------|-------------|----------|
| **GUI Framework** |
| Qt 6 (C++) | Native performance, proven for CAD, cross-platform, LGPL | C++ complexity, learning curve | 5 | ✓ |
| Electron | Fast development, web tech, cross-platform | Poor performance, large size, heavy memory | 2 | ✗ |
| GTK | LGPL, good Linux support | Weaker Windows support, less suitable for CAD | 3 | ✗ |
| **Graphics Engine** |
| Qt Graphics View | Integrated with Qt, scene graph, proven | Limited 3D support (not needed) | 5 | ✓ |
| Custom OpenGL | Maximum control, best performance | High complexity, platform differences | 4 | Future |
| **Constraint Solver** |
| Custom | Full control, simple start, license freedom | Initial effort, less mature | 4 | ✓ |
| SolveSpace | Mature, proven | GPL license, integration complexity | 3 | Future |
| **File Formats** |
| libdxfrw (DXF) | Comprehensive DXF support | GPL license (dynamic link OK) | 4 | ✓ |
| Qt SVG (SVG) | Built-in, simple, LGPL | Limited control | 5 | ✓ |
| QPdfWriter (PDF) | Built-in, simple | Limited options | 5 | ✓ |
| **Build System** |
| CMake | Industry standard, excellent Qt support | Verbose syntax | 5 | ✓ |
| Meson | Modern, fast | Less Qt integration | 3 | ✗ |

---

## Appendix B: Performance Targets Summary

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| **Rendering** |
| Frame rate (100-200 objects) | 60 fps | Frame time profiler |
| Frame rate (500 objects, full view) | 60 fps (goal), 30 fps (acceptable) | Frame time profiler |
| Frame rate (500 objects, isolation mode) | 60 fps | Frame time profiler |
| Zoom response | < 100ms | Timestamp before/after |
| Pan response | < 50ms | Timestamp before/after |
| Selection response | < 50ms | Timestamp before/after |
| **Operations** |
| Undo/redo (typical) | < 100ms | Command timer |
| Undo/redo (complex, e.g., nesting) | < 200ms | Command timer |
| Parametric update (50 objects) | < 500ms | Timer |
| Constraint solving | < 200ms | Solver timer |
| **File I/O** |
| File save (typical project, < 5MB) | < 1 second | Timer |
| File load (typical project, < 5MB) | < 2 seconds | Timer |
| Auto-save (background) | < 500ms, non-blocking | Timer, UI responsiveness check |
| DXF export | < 3 seconds | Timer |
| **Algorithms** |
| Nesting (20 pieces, greedy) | < 1 second | Timer |
| Nesting (20 pieces, optimal) | < 10 seconds | Timer |
| Nesting (50 pieces, greedy) | < 2 seconds | Timer |
| Nesting (50 pieces, optimal) | < 30 seconds | Timer |
| Seam allowance generation | < 100ms | Timer |
| Pattern grading (5 sizes) | < 5 seconds | Timer |
| **UI** |
| Application startup | < 3 seconds | Startup timer |
| Isolation mode toggle | < 100ms | Timer |
| Layer visibility toggle | < 100ms | Timer |
| **Memory** |
| Typical project (100 objects) | < 300MB | Memory profiler |
| Large project (500 objects) | < 800MB | Memory profiler |
| Maximum project (2000 objects) | < 2GB | Memory profiler |

---

## Appendix C: File Format Specifications

### Native Format (.patterncad)

**Version:** 1.0

**Format:** JSON (UTF-8)

**MIME Type:** application/x-patterncad

**Example:**
```json
{
  "version": "1.0",
  "metadata": {
    "created": "2026-01-27T10:00:00Z",
    "modified": "2026-01-27T11:30:00Z",
    "author": "User Name",
    "application": "PatternCAD 1.0"
  },
  "settings": {
    "units": "mm",
    "gridEnabled": true,
    "gridSpacing": 10,
    "snapToGrid": true
  },
  "parameters": [
    {
      "id": "param1",
      "name": "waist_width",
      "value": 800,
      "unit": "mm",
      "expression": "",
      "group": "Measurements"
    }
  ],
  "layers": [
    {
      "id": "layer1",
      "name": "Pattern Pieces",
      "color": "#0000FF",
      "visible": true,
      "locked": false,
      "zOrder": 0
    }
  ],
  "patterns": [
    {
      "id": "pattern1",
      "name": "Front Panel",
      "layer": "layer1",
      "geometry": [
        {
          "type": "line",
          "id": "line1",
          "start": {"x": 0, "y": 0},
          "end": {"x": 100, "y": 0}
        },
        {
          "type": "bezier",
          "id": "curve1",
          "p0": {"x": 100, "y": 0},
          "p1": {"x": 120, "y": 20},
          "p2": {"x": 120, "y": 80},
          "p3": {"x": 100, "y": 100}
        }
      ],
      "constraints": [
        {
          "type": "distance",
          "objects": ["line1"],
          "value": 100,
          "parameter": "param1"
        }
      ],
      "seamAllowance": {
        "enabled": true,
        "width": 10,
        "corner": "miter"
      },
      "notches": [
        {
          "position": {"x": 50, "y": 0},
          "style": "V",
          "depth": 5
        }
      ]
    }
  ]
}
```

---

**End of Architecture Document**
