# Product Requirements Document: PatternCAD

**Date:** 2026-01-27
**Author:** Product Manager
**Version:** 1.0
**Project Type:** desktop-app
**Project Level:** 3
**Status:** Draft

---

## Document Overview

This Product Requirements Document (PRD) defines the functional and non-functional requirements for PatternCAD. It serves as the source of truth for what will be built and provides traceability from requirements through implementation.

**Related Documents:**
- Product Brief: docs/product-brief.md

---

## Executive Summary

PatternCAD is an open-source, professional-grade 2D pattern design software that provides a powerful yet accessible alternative to expensive proprietary solutions like Optitex. Designed primarily for Linux with Windows support, PatternCAD democratizes access to professional patterning tools for artisans, prototypists, and hobbyists across industries beyond garment manufacturing.

Unlike existing solutions that are prohibitively expensive, platform-restricted, and overly specialized for clothing design, PatternCAD offers comprehensive 2D CAD capabilities with an intuitive interface that balances simplicity with professional-grade functionality.

---

## Product Goals

### Business Objectives

1. **Democratize Professional Patterning**: Make professional-grade pattern design tools freely available to anyone regardless of budget or platform preference

2. **Establish Linux as Viable Platform**: Prove that serious creative and technical work can be done on Linux by building a best-in-class tool with Linux-first development

3. **Build Open-Source Community**: Create a sustainable open-source project with active contributors and users across multiple industries

4. **Achieve Feature Parity**: Deliver 2D functionality comparable to commercial solutions like Optitex (excluding 3D garment-specific features)

### Success Metrics

- **Adoption**: Active user base across artisan, prototypist, and hobbyist communities
- **Functionality**: Complete implementation of core 2D patterning features
- **Stability**: Production-ready software suitable for professional use
- **Community**: Growing contributor base and positive user feedback
- **Platform Success**: Significant adoption on Linux demonstrating platform viability

---

## Functional Requirements

Functional Requirements (FRs) define **what** the system does - specific features and behaviors.

Each requirement includes:
- **ID**: Unique identifier (FR-001, FR-002, etc.)
- **Priority**: Must Have / Should Have / Could Have / Won't Have (MoSCoW)
- **Description**: What the system should do
- **Acceptance Criteria**: How to verify it's complete

---

### FR-001: Draw Basic Geometric Shapes

**Priority:** Must Have

**Description:**
Users must be able to create basic geometric shapes including straight lines, points, rectangles, circles, and arcs using mouse-based drawing tools with numerical input option.

**Acceptance Criteria:**
- [ ] User can draw straight lines by clicking start and end points
- [ ] User can place individual points at specific coordinates
- [ ] User can create rectangles by defining opposite corners
- [ ] User can draw circles by defining center and radius
- [ ] User can create arcs by defining start point, end point, and control point
- [ ] All shapes support numerical input for precise dimensions
- [ ] Shapes are visually represented in the canvas with clear rendering

**Dependencies:** None

---

### FR-002: Draw and Edit Bezier Curves

**Priority:** Must Have

**Description:**
Users must be able to create and manipulate bezier curves with control points for creating complex curved pattern pieces.

**Acceptance Criteria:**
- [ ] User can create cubic bezier curves by defining anchor and control points
- [ ] User can adjust control points after curve creation
- [ ] Curve smoothness is maintained during editing
- [ ] Curves support conversion to/from other curve types (arcs)
- [ ] Control points are visually distinct and easily selectable

**Dependencies:** FR-001

---

### FR-003: Select and Edit Vector Objects

**Priority:** Must Have

**Description:**
Users must be able to select individual or multiple vector objects and edit their properties including position, size, and visual appearance. A spacebar-based grab-and-move interaction provides quick repositioning.

**Acceptance Criteria:**
- [ ] User can select single objects by clicking
- [ ] User can select multiple objects using marquee selection or Shift+click
- [ ] **Spacebar grab-and-move**: When mouse hovers over a piece, pressing spacebar selects it and enters move mode
- [ ] **Move mode**: While in move mode, the piece is visible at all times and follows mouse movement in real-time
- [ ] **Visual feedback**: The moving piece is displayed with full visibility (not just outline or ghost)
- [ ] **Superposition allowed**: Pieces can be placed over other pieces for comparison purposes - no collision prevention
- [ ] **Drop piece**: Pressing spacebar again drops the piece at the current mouse position
- [ ] **Cancel move**: Pressing Escape during move mode cancels and returns piece to original position
- [ ] Selected objects show visual feedback (handles, highlights)
- [ ] User can also move selected objects by traditional dragging
- [ ] User can enter precise coordinates for object position
- [ ] User can modify object properties (line weight, color, style)
- [ ] Undo/redo functionality works for all edit operations

**Dependencies:** FR-001, FR-002

---

### FR-004: Define Parametric Dimensions

**Priority:** Must Have

**Description:**
Users must be able to define patterns using parameters (variables) that can be modified to update the entire pattern dynamically.

**Acceptance Criteria:**
- [ ] User can create named parameters (e.g., "waist_width", "seam_allowance")
- [ ] Parameters can be used in formulas for dimensions (e.g., "waist_width / 2")
- [ ] Changing a parameter value updates all dependent geometry automatically
- [ ] Parameters support numeric values with units (mm, cm, inches)
- [ ] Parameter panel displays all defined parameters with current values
- [ ] Parameters can be organized into groups (e.g., "Body Measurements", "Allowances")

**Dependencies:** FR-001, FR-002

---

### FR-005: Apply Geometric Constraints

**Priority:** Must Have

**Description:**
Users must be able to apply constraints between objects to maintain relationships (parallel, perpendicular, equal length, tangent, etc.).

**Acceptance Criteria:**
- [ ] User can constrain lines to be parallel or perpendicular
- [ ] User can constrain lengths to be equal
- [ ] User can constrain points to be coincident
- [ ] User can constrain curves to be tangent at connection points
- [ ] Constraints are visually indicated with symbols/icons
- [ ] Constraint solver maintains relationships during edits
- [ ] User can remove or temporarily disable constraints

**Dependencies:** FR-001, FR-002

---

### FR-006: Rotate Objects

**Priority:** Must Have

**Description:**
Users must be able to rotate pattern pieces around a specified point with precise angle control.

**Acceptance Criteria:**
- [ ] User can rotate selected objects by dragging
- [ ] User can specify exact rotation angle numerically
- [ ] User can define custom rotation center point
- [ ] Rotation preview shows before committing
- [ ] Rotation preserves object properties and constraints

**Dependencies:** FR-003

---

### FR-007: Mirror Objects

**Priority:** Must Have

**Description:**
Users must be able to create mirrored copies of pattern pieces along a defined axis.

**Acceptance Criteria:**
- [ ] User can mirror objects horizontally or vertically
- [ ] User can define custom mirror axis using two points or a line
- [ ] Mirrored copy is created as new object (original preserved)
- [ ] Mirrored objects can optionally maintain parametric link to original
- [ ] Mirror operation works on single objects or groups

**Dependencies:** FR-003

---

### FR-008: Scale Objects

**Priority:** Must Have

**Description:**
Users must be able to scale pattern pieces uniformly or non-uniformly with precise control.

**Acceptance Criteria:**
- [ ] User can scale objects by percentage or absolute dimensions
- [ ] User can scale uniformly (maintaining proportions) or non-uniformly (different X/Y)
- [ ] User can define scale origin point
- [ ] Scale preview shows before committing
- [ ] Parametric relationships are maintained after scaling

**Dependencies:** FR-003

---

### FR-009: Align and Distribute Objects

**Priority:** Should Have

**Description:**
Users should be able to align multiple objects relative to each other or distribute them evenly.

**Acceptance Criteria:**
- [ ] User can align selected objects: left, right, top, bottom, center (horizontal/vertical)
- [ ] User can distribute objects evenly with specified spacing
- [ ] Alignment reference can be selection bounds or specific object
- [ ] Preview shows alignment result before applying

**Dependencies:** FR-003

---

### FR-010: Add Seam Allowance Automatically

**Priority:** Must Have

**Description:**
Users must be able to automatically add seam allowance around pattern piece edges with configurable width and corner handling.

**Acceptance Criteria:**
- [ ] User can specify seam allowance width (in mm, cm, or inches)
- [ ] Seam allowance is added as offset outline around pattern piece
- [ ] Corner handling options: miter, round, bevel
- [ ] User can exclude specific edges from seam allowance
- [ ] Seam allowance is visually distinct (different line style/color)
- [ ] Seam allowance updates automatically if pattern shape changes

**Dependencies:** FR-001, FR-002

---

### FR-011: Manage Notches and Match Points

**Priority:** Must Have

**Description:**
Users must be able to add notches, match points, and alignment markers to pattern pieces for assembly guidance.

**Acceptance Criteria:**
- [ ] User can place notches on pattern edges (various styles: V, slit, dot)
- [ ] User can place match points with labels for piece alignment
- [ ] Notches and match points snap to edges and curves
- [ ] Match points can be linked between pieces to show relationships
- [ ] Notch depth and style are configurable

**Dependencies:** FR-001, FR-002

---

### FR-012: Grade Pattern by Size

**Priority:** Must Have

**Description:**
Users must be able to create multiple size variations of a pattern using grading rules.

**Acceptance Criteria:**
- [ ] User can define size range (e.g., XS, S, M, L, XL with measurements)
- [ ] User can specify grading rules per size (incremental changes)
- [ ] Grading can be applied to entire pattern or specific dimensions
- [ ] All sizes can be generated and exported simultaneously
- [ ] Preview shows all graded sizes overlaid or side-by-side

**Dependencies:** FR-004

---

### FR-013: Scale Pattern Proportionally

**Priority:** Must Have

**Description:**
Users must be able to scale entire patterns while maintaining proportions and parametric relationships.

**Acceptance Criteria:**
- [ ] User can scale pattern by percentage
- [ ] User can scale pattern to fit specific dimensions
- [ ] All parametric relationships are maintained after scaling
- [ ] Constraints remain valid after scaling
- [ ] Seam allowances and notches scale appropriately

**Dependencies:** FR-004, FR-008, FR-010

---

### FR-014: Automatic Nesting of Pattern Pieces

**Priority:** Should Have

**Description:**
Users should be able to automatically arrange multiple pattern pieces on a material sheet to minimize waste using intelligent nesting algorithms. The system analyzes piece geometry, applies packing algorithms, and respects user-defined constraints (grain direction, spacing, rotation limits).

**Acceptance Criteria:**
- [ ] User can specify material dimensions (width, length) with optional irregular shapes
- [ ] User can specify minimum spacing/gap between pieces (safety margin)
- [ ] User can define grain direction constraints per piece (parallel, perpendicular, any)
- [ ] User can lock rotation for specific pieces or allow 90°/180°/360° rotation
- [ ] Automatic nesting algorithm options: fast (greedy), balanced, optimal (slower)
- [ ] Algorithm attempts multiple orientations and positions to find best fit
- [ ] System calculates and displays material utilization percentage in real-time
- [ ] System shows estimated waste area visually
- [ ] User can set nesting priorities (minimize waste vs. respect grain vs. speed)
- [ ] Nesting can be re-run with different settings without losing manual adjustments
- [ ] Results can be saved as named layouts for comparison
- [ ] System handles both convex and non-convex (complex) piece shapes
- [ ] Progress indicator shows during long nesting calculations
- [ ] User can cancel nesting operation in progress

**Implementation Notes:**
- Consider algorithms: bin packing, genetic algorithms, or simplified heuristics for MVP
- Performance target: nest 20-50 pieces in under 10 seconds on modest hardware
- Provide "good enough" solution quickly, with option to continue optimizing

**Dependencies:** FR-003, FR-015

---

### FR-015: Manual Placement and Arrangement

**Priority:** Must Have

**Description:**
Users must be able to manually arrange pattern pieces on a layout sheet with intelligent assistance tools including snapping, alignment guides, and collision detection. This provides full control for users who prefer manual layout or need to adjust automatic nesting results.

**Acceptance Criteria:**
- [ ] User can define layout sheet dimensions (width × height) with units
- [ ] User can define multiple sheets within same project (e.g., multiple fabric panels)
- [ ] Layout sheet displays with visual boundary and optional grid
- [ ] Pattern pieces can be placed via drag-and-drop or spacebar grab-and-move (per FR-003)
- [ ] Smart guides appear when piece edges align with other pieces or sheet boundaries
- [ ] Smart guides show alignment: edge-to-edge, center-to-center, equal spacing
- [ ] Snap distance is configurable (default: 2-5mm depending on zoom level)
- [ ] Grid overlay can be toggled on/off with configurable spacing
- [ ] Snap-to-grid can be enabled/disabled (on by default)
- [ ] Pieces show **optional** collision/overlap warning (visual highlight) when overlapping others, but overlap is allowed
- [ ] Overlap detection is informational only: doesn't prevent piece placement
- [ ] Pieces can be locked in place individually to prevent accidental movement
- [ ] User can lock all pieces or unlock all with single action
- [ ] Distribution tools: evenly space selected pieces horizontally or vertically
- [ ] Alignment tools: align selected pieces to left/right/top/bottom/center
- [ ] Rotation snapping: snap to 15°, 45°, 90° increments during rotation (toggle-able)
- [ ] Layout displays material utilization percentage and total used area
- [ ] Layout can be zoomed and panned independently from main pattern editing view
- [ ] Ruler/scale indicators show measurements along sheet edges
- [ ] User can copy/duplicate layout sheets within same project

**Interaction Details:**
- **Spacebar Grab**: Hover over piece → Spacebar (piece selected) → Move mouse (piece follows in real-time with full visibility) → Spacebar to drop
- **Pieces are always visible during move**: Full rendering, not just outline or preview
- **Superposition is allowed**: Users can intentionally place pieces on top of each other to compare shapes, sizes, or alignments
- **Traditional Drag**: Click and hold → Drag → Release
- **Precision Move**: Select piece → Enter X,Y coordinates in properties panel
- **Rotation**: Select piece → R key → Move mouse or enter angle → Click to confirm
- **Lock/Unlock**: Right-click piece → Lock/Unlock, or use lock icon in layers panel

**Dependencies:** FR-003, FR-021, FR-023

---

### FR-016: Measure Distances and Dimensions

**Priority:** Must Have

**Description:**
Users must be able to measure distances, angles, perimeters, and areas with precision measurement tools. Measurements update dynamically as geometry changes and support multiple unit systems. This is critical for verifying pattern accuracy and ensuring proper fit.

**Acceptance Criteria:**

**Distance Measurement:**
- [ ] User can measure straight-line distance between any two points (click-click)
- [ ] User can measure distance along curves or polylines (total path length)
- [ ] Measurement displays in current unit system (mm, cm, inches) with conversion
- [ ] Measurement tool shows live preview while moving cursor
- [ ] Measurements can be saved as temporary or persistent annotations

**Angle Measurement:**
- [ ] User can measure angle between two lines (3-point angle: vertex + 2 points)
- [ ] User can measure angle at any vertex automatically
- [ ] Angle displays in degrees with decimal precision
- [ ] Option to display in radians if needed

**Perimeter/Circumference:**
- [ ] User can measure perimeter of closed shapes automatically
- [ ] User can measure perimeter of open paths (total length of all segments)
- [ ] For complex shapes, system calculates accurate curved segment lengths

**Area Measurement:**
- [ ] User can measure area of closed shapes (polygons, circles, complex curves)
- [ ] Area calculation handles curves accurately (not just straight segments)
- [ ] Area displays in current unit² (mm², cm², inch²)

**Dynamic Updates:**
- [ ] All measurements update automatically when geometry changes
- [ ] Parametric measurements update when parameters change
- [ ] User can "pin" measurements to keep them visible during work

**Unit System:**
- [ ] User can switch measurement units globally: metric (mm, cm) or imperial (inches)
- [ ] Conversion is automatic and preserves precision
- [ ] User can set default unit system in preferences
- [ ] Individual measurements can display in different units if needed

**Measurement Display:**
- [ ] Measurements appear as floating labels near measured object
- [ ] Measurement values are readable at various zoom levels (auto-scaling text)
- [ ] User can customize measurement text size, color, and format
- [ ] Measurements can be shown/hidden with layer visibility toggle

**Measurement History:**
- [ ] Recent measurements are stored in measurement panel/history
- [ ] User can recall previous measurements without re-measuring
- [ ] Measurement history cleared on file close (or optionally saved)

**Dependencies:** FR-001, FR-002

---

### FR-017: Add Dimension Annotations

**Priority:** Should Have

**Description:**
Users should be able to add professional dimension annotations to patterns for documentation, communication, and manufacturing reference. Dimensions are associative (linked to geometry) and update automatically when shapes change.

**Acceptance Criteria:**

**Linear Dimensions:**
- [ ] User can add horizontal, vertical, or aligned linear dimensions
- [ ] Dimensions include extension lines, dimension line, and arrowheads
- [ ] User can select two points/edges to dimension
- [ ] Dimension text shows measured distance in current units
- [ ] Extension lines extend beyond dimension line with configurable overshoot
- [ ] Dimension text position: centered, above, below, or manual placement

**Angular Dimensions:**
- [ ] User can add angular dimensions between two lines
- [ ] Angular dimension shows arc and angle value in degrees
- [ ] User can choose arc position (inside or outside angle)

**Radial Dimensions:**
- [ ] User can dimension circles/arcs with radius or diameter annotation
- [ ] Radius dimension shows "R" prefix, diameter shows "Ø" prefix
- [ ] Leader line points from text to circle center or arc

**Associative Behavior:**
- [ ] Dimensions auto-update when geometry changes
- [ ] If dimensioned geometry is deleted, dimension shows error or is removed
- [ ] Dimensions maintain association through move, rotate, scale operations
- [ ] User can break association to make dimension static (manual override)

**Dimension Styles:**
- [ ] Arrow styles: filled, open, closed, slash, dot, or none
- [ ] Line weight and color are configurable
- [ ] Text font, size, and style are configurable
- [ ] Dimension precision: number of decimal places (0-4)
- [ ] Dimension can include tolerance values (+/- or min/max)
- [ ] User can create named dimension styles for consistency

**Visibility and Organization:**
- [ ] Dimensions can be shown/hidden globally or per-layer
- [ ] Dimensions can be assigned to specific layers
- [ ] Dimension layer can be toggled for cleaner working view vs. documentation view
- [ ] Dimensions export to PDF and DXF with proper formatting

**Text Override:**
- [ ] User can manually override dimension text (e.g., "SEE DETAIL A")
- [ ] Override maintains visual dimension but replaces measured value
- [ ] Override is clearly indicated (color or prefix)

**Dependencies:** FR-016

---

### FR-018: Import Standard CAD Formats

**Priority:** Must Have

**Description:**
Users must be able to import patterns from standard CAD file formats (DXF, SVG) to enable interoperability with other tools, receive patterns from clients/collaborators, or migrate from existing software. Import must be robust, handle format variations, and preserve geometry accuracy.

**Acceptance Criteria:**

**DXF Import:**
- [ ] System imports DXF files (AutoCAD R12 and later versions through R2018+)
- [ ] Supported DXF entities: LINE, CIRCLE, ARC, POLYLINE, LWPOLYLINE, SPLINE, ELLIPSE, POINT
- [ ] Import preserves layer information and layer names
- [ ] Import handles multiple units: mm, cm, inches, feet (with proper scaling)
- [ ] Import detects and applies correct unit scaling automatically
- [ ] User can override unit interpretation if auto-detection is wrong
- [ ] Import handles both model space and paper space (user selects)
- [ ] Complex entities (hatches, dimensions) are converted or ignored gracefully
- [ ] Import preserves object colors and line weights where applicable
- [ ] Large DXF files (1000+ entities) import with progress indicator

**SVG Import:**
- [ ] System imports SVG files (version 1.1 and 2.0)
- [ ] Supported SVG elements: path, line, circle, ellipse, rect, polygon, polyline
- [ ] Import handles SVG transforms (translate, rotate, scale, matrix)
- [ ] Import respects SVG viewBox for proper scaling
- [ ] User can specify import scale if SVG lacks proper units
- [ ] Bezier curves and complex paths are converted to editable curves
- [ ] Import ignores or warns about unsupported elements (text, images, filters)
- [ ] Grouped objects in SVG are imported as grouped objects or layers
- [ ] Import preserves stroke width and colors where applicable

**General Import Features:**
- [ ] Import dialog allows file preview before importing
- [ ] Import options dialog: scale, units, layer handling, coordinate system
- [ ] Imported geometry is converted to native PatternCAD editable objects
- [ ] Import creates new layer(s) named after source file
- [ ] User can choose: import to new file, append to current file, or replace current
- [ ] Import validation: check for corrupt data, missing entities, unsupported features
- [ ] Import warnings/errors are clearly reported with details
- [ ] Error log shows which entities failed to import and why
- [ ] Import supports drag-and-drop of files onto application window
- [ ] Recent import locations are remembered for convenience

**Post-Import:**
- [ ] Imported geometry can be immediately edited (select, move, etc.)
- [ ] User can undo import operation if result is unexpected
- [ ] Import preserves parametric relationships if source format supports them (rare)

**Dependencies:** None

---

### FR-019: Export to Standard CAD Formats

**Priority:** Must Have

**Description:**
Users must be able to export patterns to standard formats (DXF, SVG, PDF) for plotting, cutting machines (laser, vinyl, CNC), sharing with collaborators, or use in other CAD/manufacturing tools. Export must produce clean, accurate files compatible with industry-standard software and hardware.

**Acceptance Criteria:**

**DXF Export:**
- [ ] System exports to DXF format (AutoCAD R12 for maximum compatibility, optionally R2018)
- [ ] Exported entities: LINE, CIRCLE, ARC, POLYLINE, LWPOLYLINE, SPLINE (as appropriate)
- [ ] Export preserves layer structure and layer names
- [ ] Export handles units correctly (user specifies: mm, cm, inches)
- [ ] Export includes seam allowances, notches, and annotations as separate layers
- [ ] User can choose which layers to export (e.g., exclude construction lines)
- [ ] Export produces clean geometry without duplicate or overlapping entities
- [ ] Bezier curves are converted to DXF SPLINE entities with proper control points
- [ ] Line weights and colors are preserved in DXF layer properties
- [ ] Export tested for compatibility with AutoCAD, QCAD, LibreCAD, Fusion 360

**SVG Export:**
- [ ] System exports to SVG format (version 1.1, optionally 2.0)
- [ ] Exported elements: path, line, circle, ellipse, rect as appropriate
- [ ] Export includes proper viewBox and dimensions for accurate scaling
- [ ] User can specify SVG units: px, mm, cm, inches
- [ ] Layers are exported as SVG groups with IDs
- [ ] Export preserves stroke width, colors, and fill properties
- [ ] Bezier curves exported as SVG path elements with cubic bezier commands
- [ ] Export produces clean, human-readable SVG (formatted, not minified)
- [ ] Export tested for compatibility with Inkscape, Illustrator, web browsers

**PDF Export:**
- [ ] System exports to PDF format (PDF/A for archival, or standard PDF 1.7)
- [ ] User can specify page size: A4, A3, A2, A1, A0, Letter, Tabloid, or custom
- [ ] User can choose orientation: portrait or landscape
- [ ] Export can fit pattern to page or use exact scale (1:1, 1:2, etc.)
- [ ] Multi-page export: large patterns split across multiple pages with alignment marks
- [ ] Export includes layers as PDF layers (optional layers in PDF viewer)
- [ ] User can choose what to include: pattern only, dimensions, annotations, grid
- [ ] PDF preserves vector quality (not rasterized)
- [ ] Export includes metadata: title, author, creation date, scale info
- [ ] Export tested for compatibility with Adobe Reader, plotting software, print shops

**General Export Features:**
- [ ] Export dialog with preview showing what will be exported
- [ ] Export options: selected pieces only, current layout, all pieces, specific layers
- [ ] Export presets: "For Plotting", "For Laser Cutter", "For Client Review"
- [ ] User can save custom export presets
- [ ] Export validates geometry before exporting (checks for open curves, overlaps)
- [ ] Export warnings: non-exportable elements, format limitations
- [ ] Export progress indicator for large/complex files
- [ ] Recent export locations are remembered
- [ ] Batch export: export multiple layouts or sizes in one operation
- [ ] Export filename suggestions include pattern name, date, version

**Quality Assurance:**
- [ ] Exported files are dimensionally accurate (1:1 scale verification)
- [ ] Curves are exported with sufficient precision (no visible faceting)
- [ ] Notches and markers are clearly visible in exported files
- [ ] Text annotations are readable (or converted to curves if font embedding issues)

**Dependencies:** FR-001 through FR-017

---

### FR-020: Save and Load Native Project Files

**Priority:** Must Have

**Description:**
Users must be able to save complete projects with all data (geometry, parameters, constraints, settings, history) in a native file format and reload them with full fidelity. The file format must be reliable, forward-compatible where possible, and support efficient save/load operations.

**Acceptance Criteria:**

**Save Functionality:**
- [ ] System saves projects in native file format (.patterncad, .pcad, or similar extension)
- [ ] Saved files include all geometry: points, lines, curves, shapes
- [ ] Saved files include all parameters and their values
- [ ] Saved files include all constraints and relationships
- [ ] Saved files include layers, layer properties, and object-layer assignments
- [ ] Saved files include user preferences for this project: units, grid settings, view state
- [ ] Saved files include layout sheets and piece placements
- [ ] Saved files include dimension annotations and measurements
- [ ] File format uses standard structure: JSON, XML, or binary with documented schema
- [ ] Save operation is fast: < 1 second for typical project (100-200 objects)
- [ ] User can choose "Save" (overwrite) or "Save As" (new file)
- [ ] Save dialog remembers last used directory
- [ ] Filename validation prevents invalid characters

**Load Functionality:**
- [ ] Projects load completely with all geometry, parameters, and settings restored
- [ ] Load restores view state: zoom level, pan position (optionally)
- [ ] Load restores layer visibility and lock states
- [ ] Projects load with full editing capability (all objects are editable)
- [ ] Load operation is fast: < 2 seconds for typical project
- [ ] Large projects show loading progress indicator
- [ ] Load validates file integrity before opening (checksums, version checks)
- [ ] Corrupted files show clear error message with recovery options if possible

**File Format Versioning:**
- [ ] File format includes version number in header
- [ ] Newer application versions can open older file versions (backward compatibility)
- [ ] If file is newer version than application, show warning and attempt to open
- [ ] File format is documented for future extensibility
- [ ] Breaking changes in file format are versioned clearly

**Incremental Save:**
- [ ] "Save" operation overwrites existing file quickly
- [ ] "Save As" creates new file with new name
- [ ] User is warned before overwriting existing files (except during normal "Save")
- [ ] Failed save operations don't corrupt existing file (write to temp, then rename)

**Recent Files:**
- [ ] Application maintains list of recently opened files (last 10-20)
- [ ] Recent files accessible from File menu
- [ ] Recent files list shows full path and last modified date
- [ ] Non-existent files are removed from recent list gracefully

**File Associations:**
- [ ] .patterncad files are associated with application on Windows
- [ ] Double-clicking .patterncad file opens PatternCAD (on Windows)
- [ ] Linux: .desktop file enables file manager integration
- [ ] Application icon appears for .patterncad files in file managers

**Project Metadata:**
- [ ] Saved files include metadata: creation date, last modified date, author
- [ ] Metadata is editable in project properties dialog
- [ ] Metadata is preserved through save operations

**Dependencies:** All FRs (comprehensive save of entire application state)

---

### FR-021: Provide Multi-Panel Workspace

**Priority:** Must Have

**Description:**
Users must have an efficient, customizable workspace with multiple panels providing access to tools, properties, layers, and the main canvas. The UI should balance power with clarity, avoiding overwhelming new users while giving experienced users quick access to all features.

**Acceptance Criteria:**

**Main Canvas:**
- [ ] Large central canvas displays pattern with dark or light background (user configurable)
- [ ] Canvas supports smooth pan: middle-mouse drag, two-finger trackpad, or hand tool
- [ ] Canvas supports zoom: mouse wheel, pinch gesture, zoom tool, or keyboard (+/-)
- [ ] Zoom range: 10% to 1000% with smooth interpolation
- [ ] Zoom to fit: automatically frame all objects in view (keyboard shortcut: F)
- [ ] Zoom to selection: frame selected objects (keyboard shortcut: Shift+F)
- [ ] Canvas shows origin point (0,0) with visual indicator
- [ ] Canvas displays current zoom percentage in status bar
- [ ] Canvas supports optional grid overlay (toggle-able)
- [ ] Canvas supports optional rulers on top and left edges showing measurements
- [ ] Canvas rendering is smooth and responsive (60fps target during pan/zoom)

**Tool Palette:**
- [ ] Vertical toolbar on left side with drawing and editing tool icons
- [ ] Tool icons are clear, recognizable, and have tooltips on hover
- [ ] Active tool is visually highlighted
- [ ] Tools organized into logical groups: Draw, Edit, Transform, Measure
- [ ] Tool groups can be collapsed/expanded to save space
- [ ] Toolbar can be docked on left or right, or float as separate window
- [ ] Toolbar remembers position and state across sessions

**Properties Panel:**
- [ ] Panel on right side shows properties of selected object(s)
- [ ] Properties update dynamically when selection changes
- [ ] Properties are organized into sections: Position, Size, Style, Constraints, Parameters
- [ ] Sections can be collapsed/expanded
- [ ] Numeric properties support direct input with unit notation (e.g., "5cm", "2.5in")
- [ ] Properties include: X/Y position, width/height, rotation angle, line weight, color
- [ ] For multiple selection, properties show common values or "Mixed" indicator
- [ ] Properties panel supports undo/redo when values are changed
- [ ] Panel can be hidden to maximize canvas space

**Layers Panel:**
- [ ] Panel displays list of layers with visibility toggles (eye icon)
- [ ] User can create, rename, delete, duplicate layers
- [ ] Layers can be reordered by drag-and-drop
- [ ] Current active layer is highlighted
- [ ] Layers show lock status with lock icon (toggle to lock/unlock)
- [ ] Layer color indicator helps identify objects by layer
- [ ] Right-click layer for context menu: rename, delete, merge, properties
- [ ] Layer count and object count per layer displayed
- [ ] Panel can be docked or floated

**Parameters Panel:**
- [ ] Panel displays all defined parameters with current values
- [ ] User can add, edit, delete parameters
- [ ] Parameters organized into user-defined groups/categories
- [ ] Parameter values can be edited inline with immediate update
- [ ] Parameters show units and constraints (min/max values)
- [ ] Panel shows which objects/dimensions use each parameter (dependency tracking)
- [ ] Panel can be hidden when not working with parametric designs

**Measurement Panel (optional):**
- [ ] Panel shows list of active measurements
- [ ] Measurements can be cleared individually or all at once
- [ ] Clicking measurement in list highlights corresponding objects on canvas
- [ ] Panel displays measurement history

**Status Bar:**
- [ ] Bottom status bar shows cursor coordinates (X, Y) in current units
- [ ] Status bar shows current zoom percentage
- [ ] Status bar shows current tool name and brief usage hint
- [ ] Status bar shows selection count (e.g., "3 objects selected")
- [ ] Status bar shows file modification status (saved/unsaved)
- [ ] Right side of status bar shows current units (mm/cm/inches) - click to change

**Panel Management:**
- [ ] All panels can be shown/hidden via View menu or keyboard shortcuts
- [ ] Panels can be docked to edges or floated as separate windows
- [ ] Panel sizes are adjustable by dragging splitters
- [ ] Workspace layout can be saved as presets (e.g., "Drafting", "Review", "Minimal")
- [ ] User can reset workspace to default layout
- [ ] Workspace state persists across application restarts

**Menu Bar:**
- [ ] Standard menu bar: File, Edit, View, Draw, Modify, Tools, Window, Help
- [ ] Menus include keyboard shortcuts displayed next to commands
- [ ] Recent files accessible from File menu
- [ ] Context-sensitive menus (right-click) provide quick access to relevant commands

**Toolbar (optional top toolbar):**
- [ ] Quick access toolbar with common commands: New, Open, Save, Undo, Redo
- [ ] Toolbar is customizable (user can add/remove buttons)
- [ ] Toolbar can be shown/hidden

**Responsive Design:**
- [ ] UI adapts gracefully to different window sizes
- [ ] Minimum window size: 1280×720 pixels (recommended)
- [ ] Panels auto-collapse or become scrollable on smaller screens
- [ ] UI elements scale appropriately on high-DPI displays (4K, Retina)

**Dependencies:** None (foundational UI requirement)

---

### FR-022: Support Keyboard Shortcuts

**Priority:** Must Have

**Description:**
Users must be able to perform common operations using keyboard shortcuts for efficiency and speed. Shortcuts should follow industry conventions where possible (CAD standards, OS standards) and be fully customizable for power users.

**Acceptance Criteria:**

**Standard Shortcuts (non-customizable):**
- [ ] Ctrl+N: New file
- [ ] Ctrl+O: Open file
- [ ] Ctrl+S: Save file
- [ ] Ctrl+Shift+S: Save As
- [ ] Ctrl+Z: Undo
- [ ] Ctrl+Y or Ctrl+Shift+Z: Redo
- [ ] Ctrl+X: Cut
- [ ] Ctrl+C: Copy
- [ ] Ctrl+V: Paste
- [ ] Ctrl+A: Select All
- [ ] Delete or Backspace: Delete selected objects
- [ ] Ctrl+D: Duplicate selection
- [ ] Ctrl+Q: Quit application
- [ ] F1: Help

**Tool Shortcuts:**
- [ ] L: Line tool
- [ ] C: Circle tool
- [ ] R: Rectangle tool
- [ ] A: Arc tool
- [ ] B: Bezier curve tool
- [ ] P: Point tool
- [ ] M: Move tool
- [ ] S: Select tool (pointer)
- [ ] H: Hand tool (pan)
- [ ] Z: Zoom tool

**Transform Shortcuts:**
- [ ] Ctrl+R: Rotate selected objects
- [ ] Ctrl+M: Mirror selected objects
- [ ] Ctrl+T: Scale/Transform selected objects
- [ ] Ctrl+G: Group selected objects
- [ ] Ctrl+Shift+G: Ungroup selected objects

**View Shortcuts:**
- [ ] +/= or Ctrl+Plus: Zoom in
- [ ] - or Ctrl+Minus: Zoom out
- [ ] F: Fit all (zoom to fit all objects)
- [ ] Shift+F: Fit selection (zoom to fit selected objects)
- [ ] Ctrl+0: Zoom to 100% (actual size)
- [ ] Ctrl+1: Zoom to fit width
- [ ] Ctrl+2: Zoom to fit height

**Special Interaction Shortcuts:**
- [ ] Spacebar: Grab-and-move piece (per FR-003) - hover, press space, move, press space again
- [ ] Escape: Cancel current operation, deselect all, or exit current tool
- [ ] Enter: Confirm current operation
- [ ] Tab: Cycle through input fields in dialogs or properties panel
- [ ] Shift+Tab: Cycle through input fields in reverse

**Layer Shortcuts:**
- [ ] Ctrl+L: New layer
- [ ] Ctrl+Shift+L: Show/hide layers panel
- [ ] Ctrl+]: Move selection to layer above
- [ ] Ctrl+[: Move selection to layer below

**Snapping and Grid Shortcuts:**
- [ ] G: Toggle grid visibility
- [ ] Ctrl+Shift+G: Toggle snap to grid
- [ ] Ctrl+': Toggle smart guides/snapping

**Modifier Keys:**
- [ ] Shift: Constrain proportions (e.g., Shift+drag = perfect circle, square, or constrained line angle)
- [ ] Alt: Duplicate while dragging
- [ ] Ctrl: Precision mode (smaller increments) or override snap
- [ ] Shift+Drag on rotation: Snap to 15° increments
- [ ] Ctrl+Click: Add to selection
- [ ] Shift+Click: Add to or remove from selection (toggle)

**Measurement Shortcuts:**
- [ ] D: Distance measurement tool
- [ ] Ctrl+Shift+M: Open measurement panel

**Shortcut Customization:**
- [ ] All tool and command shortcuts are customizable in Preferences
- [ ] Shortcut editor shows current key assignments and conflicts
- [ ] User can search for commands and assign shortcuts
- [ ] User can reset to default shortcuts
- [ ] Custom shortcut profiles can be exported/imported
- [ ] Shortcuts don't conflict with OS shortcuts (warning if conflict detected)

**Platform Differences:**
- [ ] Windows: Ctrl key for commands
- [ ] Linux: Ctrl key for commands
- [ ] Shortcuts document shows platform-specific keys clearly
- [ ] Application detects OS and displays appropriate shortcuts in menus

**Shortcut Display:**
- [ ] Keyboard shortcuts visible in menu bar next to commands
- [ ] Tool tooltips show keyboard shortcut
- [ ] Cheat sheet/reference card available: Help → Keyboard Shortcuts
- [ ] Context-sensitive help shows relevant shortcuts

**Chord Shortcuts (optional):**
- [ ] Allow multi-key sequences like Vim (e.g., "g c" for grading, "e x" for export)
- [ ] Chord mode is optional and can be disabled

**Dependencies:** FR-001 through FR-021

---

### FR-023: Provide Layers and Object Organization

**Priority:** Must Have

**Description:**
Users must be able to organize pattern pieces, construction lines, annotations, and other elements into layers for better management, visibility control, and workflow organization. Layers enable complex projects to remain organized and allow users to work on specific aspects without visual clutter.

**Acceptance Criteria:**

**Layer Creation and Management:**
- [ ] User can create new layers with descriptive names
- [ ] User can rename layers at any time (double-click or context menu)
- [ ] User can delete layers (with confirmation if layer contains objects)
- [ ] User can duplicate layers (creates new layer with copy of all objects)
- [ ] Default layer "Layer 0" or "Main" is created automatically for new projects
- [ ] Maximum layer count: 100 layers (sufficient for complex projects)

**Layer Properties:**
- [ ] Each layer has a name (max 50 characters, unique within project)
- [ ] Each layer has a color indicator (for visual identification)
- [ ] Each layer has visibility state: shown or hidden (eye icon toggle)
- [ ] Each layer has lock state: unlocked or locked (lock icon toggle)
- [ ] Locked layers: objects visible but not selectable or editable
- [ ] Each layer has print/export state: include or exclude from output

**Object-Layer Assignment:**
- [ ] New objects are created on the current active layer
- [ ] User can change active layer by clicking layer in layers panel
- [ ] Active layer is highlighted in layers panel
- [ ] User can move selected objects to different layer: drag to layer, context menu, or shortcut
- [ ] User can move objects between layers without losing object properties
- [ ] Multi-select objects can be moved to layer even if they're on different layers currently

**Layer Visibility:**
- [ ] Clicking eye icon toggles layer visibility
- [ ] Hidden layer objects don't appear on canvas
- [ ] Hidden layer objects are not selectable
- [ ] Hidden layer objects are still included in file save
- [ ] User can hide/show all layers with single command
- [ ] User can isolate layer: hide all others, show only selected layer
- [ ] Layer visibility state is saved with project file

**Layer Locking:**
- [ ] Clicking lock icon toggles layer lock state
- [ ] Locked layer objects are visible but greyed out or shown with visual indicator
- [ ] Locked layer objects cannot be selected or modified
- [ ] Locked layers protect against accidental edits
- [ ] User can lock/unlock all layers with single command
- [ ] Layer lock state is saved with project file

**Layer Ordering:**
- [ ] Layers have drawing order (z-order): top layer draws on top, bottom layer draws behind
- [ ] User can reorder layers by dragging in layers panel
- [ ] Objects on higher layers appear in front of objects on lower layers
- [ ] Layer order affects both display and selection (top layer objects selected first)

**Layer Display in Layers Panel:**
- [ ] Layers panel shows list of all layers
- [ ] Each layer row shows: visibility icon, lock icon, color indicator, layer name
- [ ] Layer row shows object count (e.g., "12 objects")
- [ ] Current active layer is highlighted with background color or border
- [ ] Layers can be collapsed/expanded if they contain sub-layers (future: layer groups)

**Layer Operations:**
- [ ] Right-click layer for context menu: Rename, Delete, Duplicate, Select All Objects, Lock/Unlock, Show/Hide
- [ ] "Select All Objects" selects all objects on that layer
- [ ] "Merge Layers": combine two or more layers into one
- [ ] "Flatten All": merge all layers into single layer (with confirmation)

**Predefined Layer Use Cases:**
- [ ] User typically creates layers like: "Pattern Pieces", "Construction Lines", "Seam Allowance", "Annotations", "Grading", "Layout"
- [ ] Application can suggest or auto-create standard layers (optional)

**Layer Colors:**
- [ ] Layer color is used to tint or identify objects on that layer (optional visual mode)
- [ ] User can assign any color to layer from color picker
- [ ] Layer color doesn't override object's own color (if set)

**Layer Search/Filter:**
- [ ] User can search layers by name (if many layers exist)
- [ ] User can filter layers: show only visible, show only locked, etc.

**Performance:**
- [ ] Layer visibility toggle is instant (< 100ms)
- [ ] Reordering layers updates display smoothly
- [ ] Large number of layers (50+) doesn't impact performance

**Dependencies:** FR-003 (object selection)

---

### FR-024: Implement Undo/Redo System

**Priority:** Must Have

**Description:**
Users must be able to undo and redo all editing operations with unlimited history depth (limited only by memory). The undo system must be reliable, fast, and handle all types of operations including creation, deletion, modification, and property changes. This is critical for user confidence and error recovery.

**Acceptance Criteria:**

**Undo Functionality:**
- [ ] All editing operations are undoable: create, delete, modify, move, rotate, scale, etc.
- [ ] Property changes are undoable: color, line weight, layer assignment, parameters
- [ ] Undo works via Ctrl+Z keyboard shortcut
- [ ] Undo works via Edit → Undo menu item
- [ ] Undo works via toolbar button (optional)
- [ ] Undo history depth is unlimited (constrained by available memory)
- [ ] Typical undo history stores last 100-500 operations without memory issues

**Redo Functionality:**
- [ ] Redo reverses an undo operation: restores what was undone
- [ ] Redo works via Ctrl+Y (or Ctrl+Shift+Z) keyboard shortcut
- [ ] Redo works via Edit → Redo menu item
- [ ] Redo works via toolbar button (optional)
- [ ] Redo history is cleared when a new operation is performed after undo

**Undo/Redo Behavior:**
- [ ] Each undo step goes back one operation (not multiple)
- [ ] Undo/redo operations are fast: < 50ms for typical operations
- [ ] Large operations (e.g., nesting 50 pieces) undo in < 200ms
- [ ] Undo/redo updates canvas and properties panel immediately
- [ ] Undo/redo respects selection: if operation changed selection, undo restores previous selection

**Operation Grouping:**
- [ ] Related operations can be grouped into single undo step
- [ ] Example: moving 10 objects together = 1 undo step (not 10)
- [ ] Example: parametric update that affects multiple objects = 1 undo step
- [ ] Example: applying transform with preview = 1 undo step (not undo for each preview)

**Undo History Display:**
- [ ] Menu shows current undo action: "Undo Move Object" (not just "Undo")
- [ ] Menu shows current redo action: "Redo Move Object" (not just "Redo")
- [ ] Optional undo history panel shows list of recent operations
- [ ] User can click on specific history item to undo/redo to that point
- [ ] History panel shows operation type and timestamp

**Non-Undoable Operations:**
- [ ] File operations are not undoable: Save, Open, Close (these are intentional file actions)
- [ ] View operations are not undoable: Pan, Zoom, changing active layer (these don't modify data)
- [ ] Preferences changes are not undoable (separate from document state)

**Undo Across Sessions:**
- [ ] Undo history is NOT persisted across file close/open (history cleared on load)
- [ ] Rationale: prevents confusion and file size bloat

**Memory Management:**
- [ ] Undo system uses efficient memory: stores differences (delta), not full copies
- [ ] Very old undo history can be pruned if memory limit is reached (configurable)
- [ ] User is warned if undo history is pruned due to memory constraints
- [ ] Memory limit for undo: configurable in preferences (default: 100-200 MB)

**Edge Cases:**
- [ ] Undo after deleting object: object is restored with all properties
- [ ] Undo after creating object: object is removed completely
- [ ] Undo after layer delete: layer and all objects are restored
- [ ] Undo handles circular dependencies in parametric operations gracefully

**Undo Consistency:**
- [ ] Undo system never leaves document in invalid state
- [ ] If undo fails (extremely rare), error is logged and user is notified
- [ ] Application remains stable even if undo encounters unexpected state

**Testing Requirements:**
- [ ] Undo/redo tested with all operation types
- [ ] Stress test: 500+ undo operations without issues
- [ ] Undo/redo tested with complex parametric patterns

**Dependencies:** All editing FRs (FR-001 through FR-023)

---

### FR-025: Support Cross-Platform Consistency

**Priority:** Must Have

**Description:**
Application must work identically on Linux (primary target) and Windows with consistent UI, behavior, performance, and file compatibility. Users should be able to switch platforms seamlessly without relearning the tool or encountering platform-specific limitations.

**Acceptance Criteria:**

**Platform Support:**
- [ ] Application runs natively on Linux: Ubuntu 20.04+, Fedora 35+, Debian 11+, Manjaro, and other major distributions
- [ ] Application runs natively on Windows: Windows 10 (64-bit) and Windows 11
- [ ] Both platforms use same codebase (not separate ports)
- [ ] Both platforms built from same source with platform-specific build configurations

**UI Consistency:**
- [ ] User interface appearance is visually consistent across platforms
- [ ] Window layout, panel positions, and controls are identical
- [ ] Fonts and text rendering are clear and consistent (accounting for OS font rendering differences)
- [ ] Icons and toolbar appearance match across platforms
- [ ] Dialogs and menus have same structure and options
- [ ] High-DPI display support on both platforms (4K, Retina)

**Functional Consistency:**
- [ ] All features work identically on both platforms (no platform-exclusive features)
- [ ] Drawing tools behave the same: same precision, same rendering
- [ ] Parametric system produces identical results
- [ ] Nesting algorithm produces same results given same input
- [ ] Import/export produces identical files on both platforms
- [ ] File format is 100% compatible: files created on Linux open on Windows and vice versa

**Keyboard Shortcuts:**
- [ ] Keyboard shortcuts work on both platforms
- [ ] Platform differences handled gracefully: Ctrl (Linux/Windows) vs. Cmd (if future macOS)
- [ ] Shortcuts don't conflict with OS-level shortcuts
- [ ] Shortcut display in menus shows correct keys for current platform

**Performance Consistency:**
- [ ] Application performance is comparable on similar hardware across platforms
- [ ] Rendering, pan, zoom are smooth (60fps) on both platforms
- [ ] No significant performance differences in core operations
- [ ] Large file handling (1000+ objects) performs similarly

**File Paths and Filesystem:**
- [ ] File paths handled correctly: forward slashes (Linux) vs. backslashes (Windows)
- [ ] File dialogs use native OS dialogs (consistent with OS look and feel)
- [ ] Recent files list handles paths correctly across platforms
- [ ] Application handles case-sensitive (Linux) vs. case-insensitive (Windows) filesystems

**Installation:**
- [ ] Linux: Distributed as AppImage, .deb, .rpm, or Flatpak (or multiple formats)
- [ ] Windows: Distributed as .exe installer or portable .zip
- [ ] Installation is straightforward with clear instructions
- [ ] Application integrates with OS: file associations, start menu (Windows), .desktop file (Linux)

**Dependencies and Libraries:**
- [ ] All required libraries are bundled or have clear installation instructions
- [ ] No platform-specific library conflicts
- [ ] Application doesn't require manual dependency installation

**Testing:**
- [ ] Automated tests run on both platforms in CI/CD pipeline
- [ ] Manual testing on both platforms before each release
- [ ] Platform-specific bugs are tracked and resolved with equal priority

**User Documentation:**
- [ ] Documentation clearly indicates any platform-specific instructions
- [ ] Screenshots show both platforms where relevant
- [ ] Installation guides for both platforms

**Known Acceptable Differences:**
- [ ] Window decorations match OS style (expected and acceptable)
- [ ] Native file dialogs look different (expected)
- [ ] Default font may differ based on OS (acceptable if readable)
- [ ] System integration features use OS-specific mechanisms (expected)

**Platform Priority:**
- [ ] Linux is primary development and testing platform
- [ ] Windows is secondary but must maintain full feature parity
- [ ] Platform-specific bugs on either platform are high priority

**Dependencies:** All FRs (cross-cutting requirement affecting entire application)

---

### FR-026: Auto-Save with Recovery Files

**Priority:** Must Have

**Description:**
System must automatically save work at regular intervals, creating separate recovery files that can be quickly restored in case of crash, power loss, or accidental data loss. Auto-save must be reliable, non-intrusive, and give users confidence that their work is protected.

**Acceptance Criteria:**

**Auto-Save Configuration:**
- [ ] Auto-save is enabled by default
- [ ] Auto-save interval is configurable: 1, 2, 5, 10, 15, 30 minutes (default: 3-5 minutes)
- [ ] User can disable auto-save with clear warning about data loss risk
- [ ] Auto-save preferences are accessible in Settings/Preferences
- [ ] Auto-save can be temporarily paused during intensive operations (user choice)

**Auto-Save Behavior:**
- [ ] Auto-save triggers automatically at configured interval
- [ ] Auto-save only happens if document has unsaved changes (not every interval)
- [ ] Auto-save runs in background thread: doesn't block user interaction
- [ ] User can continue working during auto-save (no interruption)
- [ ] Auto-save is fast: < 500ms for typical project, doesn't cause UI lag

**Recovery File Management:**
- [ ] Auto-save creates separate recovery files (e.g., `.patterncad.autosave`, `.patterncad.backup-001`)
- [ ] Recovery files stored in configurable location: same folder as original, temp folder, or custom folder
- [ ] Default recovery location: system temp directory or application data directory
- [ ] Recovery files are timestamped: `project-name-2026-01-27-14-30-00.autosave`
- [ ] System keeps last 5-10 recovery file versions (configurable)
- [ ] Older recovery files are automatically deleted when limit is reached (FIFO)
- [ ] Recovery file naming clearly indicates it's an auto-save, not a manual save

**Crash Recovery:**
- [ ] On application restart after crash, system detects recovery files
- [ ] Recovery dialog appears automatically showing available recovery files
- [ ] Recovery dialog shows: filename, timestamp, file size for each recovery file
- [ ] User can preview recovery file before restoring (optional)
- [ ] User can choose which recovery file to restore (most recent or older version)
- [ ] User can choose to discard recovery files and start fresh
- [ ] Restored recovery file opens as unsaved document (requires Save As)

**Recovery File Cleanup:**
- [ ] Recovery files are automatically deleted after successful manual save
- [ ] Recovery files are kept if application closes normally without manual save (protected until next session)
- [ ] User can manually clean up old recovery files in preferences
- [ ] Recovery files from abandoned projects are cleaned up after configurable time (e.g., 7 days)
- [ ] Cleanup doesn't delete recovery files for currently open projects

**User Feedback:**
- [ ] Status bar or notification shows "Auto-saved" briefly when auto-save completes
- [ ] Auto-save indicator is subtle: doesn't interrupt workflow
- [ ] If auto-save fails (disk full, permission error), user is notified immediately
- [ ] Auto-save failure notification includes reason and suggested action

**Auto-Save vs. Manual Save:**
- [ ] Auto-save does NOT replace manual save (separate feature)
- [ ] File remains "unsaved" until user manually saves (title bar shows asterisk or "unsaved")
- [ ] Manual save creates the real project file, auto-save creates recovery files
- [ ] User is still prompted to save on exit if changes made since last manual save

**File Size Considerations:**
- [ ] Auto-save uses efficient format: same as native format or compressed
- [ ] Large projects (> 10MB) show brief progress indicator during auto-save
- [ ] Auto-save doesn't create excessive disk usage (recovery file limit prevents bloat)

**Edge Cases:**
- [ ] Auto-save handles situation where original file is on read-only media (recovery to temp location)
- [ ] Auto-save handles network drives or slow storage gracefully (extended timeout)
- [ ] Auto-save handles disk full condition: warns user and retries later
- [ ] Auto-save handles permission errors: warns user and suggests solution

**Recovery File Security:**
- [ ] Recovery files have same permissions as original file
- [ ] Recovery files are not world-readable (Linux: 600 or 644)
- [ ] Recovery files in temp directory follow OS temp file security practices

**Testing:**
- [ ] Auto-save tested under normal operation
- [ ] Crash recovery tested: kill application process, restart, verify recovery
- [ ] Disk full scenario tested
- [ ] Multiple recovery file versions tested
- [ ] Large project auto-save performance tested

**Dependencies:** FR-020 (Save and Load Native Project Files)

---

### FR-027: Isolate Selected Pieces for Performance

**Priority:** Must Have

**Description:**
Users must be able to isolate selected pattern pieces to work on them without visual clutter or performance impact from hundreds of other pieces. This is similar to Blender's isolation mode and is critical for complex projects with many pieces.

**Acceptance Criteria:**

**Isolation Mode Activation:**
- [ ] User selects one or more pieces
- [ ] Pressing **Tab** key enters isolation mode
- [ ] Only selected pieces remain visible on canvas
- [ ] All other pieces are hidden (not deleted, just hidden)
- [ ] Visual indicator shows isolation mode is active (e.g., orange border, status bar message)
- [ ] Isolation mode name shows in status bar: "Isolation: 3 pieces"

**Working in Isolation Mode:**
- [ ] User can edit, move, transform isolated pieces normally
- [ ] User can draw new objects (they belong to isolation set)
- [ ] User can use all tools: drawing, measuring, constraints, etc.
- [ ] Canvas rendering is fast (only isolated pieces rendered)
- [ ] Zoom, pan work normally
- [ ] User can add more pieces to isolation: select piece and Tab again
- [ ] User can remove pieces from isolation: deselect and Tab

**Exiting Isolation Mode:**
- [ ] Pressing **Shift+Tab** exits isolation mode
- [ ] All pieces become visible again
- [ ] Changes made in isolation mode are preserved
- [ ] View returns to previous zoom/pan state (or stays at current)
- [ ] Status bar shows "All pieces visible" or similar

**Multi-Level Isolation (Optional for v1.0):**
- [ ] User can isolate within isolation (nested isolation)
- [ ] Shift+Tab exits one level at a time
- [ ] Breadcrumb shows isolation depth

**Visual Feedback:**
- [ ] Isolation mode indicated with visual cue: border color, background tint, or icon
- [ ] Layers panel shows which pieces are isolated (highlighted or filtered)
- [ ] Easy to see isolation is active (prevent confusion)

**Performance Benefits:**
- [ ] With 300 pieces total, isolating 5 pieces gives same performance as 5-piece project
- [ ] Rendering only isolated pieces reduces memory and GPU usage
- [ ] Pan/zoom remains 60fps in isolation mode regardless of total project size

**Keyboard Shortcuts:**
- [ ] Tab: Enter isolation mode (or add to isolation if already in it)
- [ ] Shift+Tab: Exit isolation mode (return to full view)
- [ ] Shortcuts are configurable in preferences

**Alternative Access:**
- [ ] Menu: View → Isolate Selected (Tab)
- [ ] Menu: View → Exit Isolation (Shift+Tab)
- [ ] Toolbar button (optional)
- [ ] Right-click selected pieces → Isolate

**Edge Cases:**
- [ ] If nothing selected when Tab pressed: show message "Select pieces to isolate"
- [ ] If in isolation mode and all pieces deleted: auto-exit isolation
- [ ] Save/Load preserves which pieces are isolated (or exits isolation on load)
- [ ] Undo/Redo works correctly with isolation mode changes

**Dependencies:** FR-003 (selection), FR-023 (layers)

---

## Non-Functional Requirements

Non-Functional Requirements (NFRs) define **how** the system performs - quality attributes and constraints.

---

### NFR-001: Performance - Rendering and Canvas Response

**Priority:** Must Have

**Description:**
The application must provide smooth, responsive rendering and canvas interaction to ensure professional workflow without frustration. Users must be able to pan, zoom, and edit patterns fluidly even with moderately complex projects.

**Acceptance Criteria:**
- [ ] Canvas rendering maintains 60fps during pan and zoom operations
- [ ] Canvas with 100-200 objects renders smoothly without lag
- [ ] Zoom in/out response time: < 100ms
- [ ] Pan response time: < 50ms (immediate)
- [ ] Object selection response: < 50ms
- [ ] Spacebar grab-and-move: piece follows mouse with < 16ms latency (60fps)
- [ ] Undo/redo operations complete in < 100ms for typical operations
- [ ] Application starts in < 3 seconds on modest hardware
- [ ] File open for typical project (< 5MB): < 2 seconds

**Rationale:**
Poor performance breaks concentration and makes professional work frustrating. CAD applications must feel responsive.

---

### NFR-002: Performance - Complex Pattern Handling

**Priority:** Must Have

**Description:**
The application must handle complex patterns with many objects, curves, and parametric relationships without significant performance degradation.

**Acceptance Criteria:**
- [ ] Projects with 500+ objects remain responsive (< 10% performance degradation)
- [ ] Projects with 1000+ objects remain usable (30fps minimum)
- [ ] Bezier curves render smoothly at all zoom levels (no visible faceting)
- [ ] Parametric updates affecting 50+ objects complete in < 500ms
- [ ] Constraint solver resolves typical constraint networks in < 200ms
- [ ] Auto-save for large projects (10MB+): < 2 seconds, non-blocking
- [ ] Nesting algorithm for 20 pieces: < 10 seconds
- [ ] Nesting algorithm for 50 pieces: < 30 seconds

**Rationale:**
Professional patterns can be complex. Performance must scale reasonably with project complexity.

---

### NFR-003: Security - File and Data Protection

**Priority:** Should Have

**Description:**
While PatternCAD is a desktop application without network features, it must protect user data from accidental loss and respect file permissions.

**Acceptance Criteria:**
- [ ] Auto-save prevents data loss from crashes (FR-026 implementation)
- [ ] File save operations are atomic: temp file write → rename (no corruption on crash)
- [ ] Recovery files have appropriate permissions (Linux: 600, not world-readable)
- [ ] Application doesn't write sensitive data to system logs
- [ ] Application respects OS file permissions (read-only files can't be overwritten)
- [ ] No crash can corrupt existing saved files
- [ ] User data stored locally only (no telemetry or cloud sync in v1.0)

**Rationale:**
Users must trust that their work is protected and private.

---

### NFR-004: Security - Input Validation

**Priority:** Must Have

**Description:**
The application must validate all user inputs and file imports to prevent crashes, corruption, or unexpected behavior.

**Acceptance Criteria:**
- [ ] Numeric inputs validated: reject invalid values, enforce min/max limits
- [ ] File imports validated: reject corrupted or malformed files gracefully
- [ ] DXF/SVG import handles malformed files without crashing
- [ ] Invalid parameter formulas show clear error messages (no silent failures)
- [ ] File path inputs sanitized to prevent directory traversal
- [ ] Maximum input string lengths enforced (prevent buffer issues)
- [ ] Invalid constraint configurations detected and reported

**Rationale:**
Robust input validation prevents crashes and ensures application stability.

---

### NFR-005: Scalability - Project Size Limits

**Priority:** Must Have

**Description:**
The application must define and support reasonable limits for project size and complexity that meet professional use cases.

**Acceptance Criteria:**
- [ ] Support projects with up to 2000 objects without major performance issues
- [ ] Support projects with up to 100 layers
- [ ] Support projects with up to 200 parameters
- [ ] Support projects with up to 500 constraints
- [ ] Support file sizes up to 100MB
- [ ] Support layout sheets up to 10 meters × 10 meters
- [ ] Support bezier curves with up to 100 control points
- [ ] Undo history supports up to 500 operations or 200MB memory (whichever first)

**Rationale:**
Clear limits prevent unexpected failures. Limits are set well above typical professional use.

---

### NFR-006: Scalability - Multi-Pattern Projects

**Priority:** Must Have

**Description:**
Users must be able to work with hundreds of pattern pieces within a single project efficiently. Performance optimization through selective display is essential for complex projects.

**Acceptance Criteria:**
- [ ] Support up to 300 pattern pieces in single project with good performance
- [ ] Support up to 500 pattern pieces with acceptable performance (using isolation mode)
- [ ] Support up to 10 size grades per pattern
- [ ] Grading operations generate all sizes in < 5 seconds
- [ ] Batch export of all sizes completes in reasonable time (< 1 minute for 10 sizes)
- [ ] Memory usage scales linearly with project size (no exponential growth)
- [ ] Isolation mode enables work on subset of pieces without rendering all (see FR-027)
- [ ] Switching between full view and isolation mode is instant (< 100ms)

**Rationale:**
Professional patterns (especially for complex products) can easily reach hundreds of pieces. Isolation mode provides performance when needed.

---

### NFR-007: Reliability - Crash Prevention

**Priority:** Must Have

**Description:**
The application must be stable and prevent crashes during normal operation. Critical operations must never cause data loss.

**Acceptance Criteria:**
- [ ] No crashes during typical workflow operations (draw, edit, transform, save)
- [ ] Application handles out-of-memory gracefully: warning message, not crash
- [ ] Application handles disk full gracefully: error message, not crash
- [ ] Invalid parametric formulas don't crash: show error, disable parameter
- [ ] Circular constraint dependencies detected and handled (no infinite loops)
- [ ] Import of extremely large files (> 100MB) handled gracefully: warning or rejection
- [ ] Target: < 1 crash per 100 hours of typical use in beta testing

**Rationale:**
Crashes destroy user trust and cause data loss even with auto-save.

---

### NFR-008: Reliability - Error Recovery

**Priority:** Must Have

**Description:**
When errors occur, the application must recover gracefully, inform the user clearly, and preserve work where possible.

**Acceptance Criteria:**
- [ ] All error messages are clear, specific, and actionable (not "Error 0x8472")
- [ ] Errors include: what happened, why it happened, what user can do
- [ ] Failed operations don't leave document in invalid state
- [ ] Transaction-like behavior: operation succeeds completely or rolls back
- [ ] Auto-save recovery works reliably after crash (tested regularly)
- [ ] Failed file imports don't corrupt current project
- [ ] Network/USB drive disconnection during save: clear error, file not corrupted

**Rationale:**
Errors are inevitable. Good error handling makes them manageable rather than catastrophic.

---

### NFR-009: Usability - User Interface Clarity

**Priority:** Must Have

**Description:**
The user interface must be clear, intuitive, and accessible to users with varying CAD experience. The learning curve should be gentle but not limiting for power users.

**Acceptance Criteria:**
- [ ] New users can create a simple pattern within 30 minutes (with tutorial)
- [ ] All tools have clear, descriptive names and icons
- [ ] Tooltips appear for all tools and buttons (< 1 second hover delay)
- [ ] Error messages guide users toward solutions (not just state problems)
- [ ] Keyboard shortcuts displayed in tooltips and menus
- [ ] Visual feedback for all interactions: hover states, selection highlights, active tools
- [ ] Undo/redo actions described in menus ("Undo Move Object", not just "Undo")
- [ ] Properties panel groups related settings logically
- [ ] No hidden features: all capabilities accessible via UI (shortcuts are additions, not requirements)

**Rationale:**
PatternCAD must be accessible to artisans and makers, not just CAD experts.

---

### NFR-010: Usability - Accessibility Standards

**Priority:** Should Have

**Description:**
The application should follow basic accessibility guidelines to be usable by people with disabilities where practical for a desktop CAD application.

**Acceptance Criteria:**
- [ ] High contrast mode available for low vision users
- [ ] UI text is readable: minimum 10pt font, scalable to 14pt
- [ ] Color is not the only indicator: shapes/icons also distinguish states
- [ ] Keyboard navigation works for all major functions (no mouse-only features)
- [ ] Tab order follows logical flow through UI
- [ ] Tooltips and labels provide text alternatives for icons
- [ ] Zoom supports very high levels (200%+) for detail work

**Rationale:**
Basic accessibility opens PatternCAD to more users without major development cost.

---

### NFR-011: Usability - Cross-Platform Consistency

**Priority:** Must Have

**Description:**
User experience must be consistent across Linux and Windows so users can switch platforms without relearning. (Note: This overlaps with FR-025 but focuses on UX quality rather than functional parity)

**Acceptance Criteria:**
- [ ] UI layout identical on Linux and Windows
- [ ] All workflows work the same way on both platforms
- [ ] File format 100% compatible (no platform-specific quirks)
- [ ] Performance comparable on equivalent hardware
- [ ] Documentation doesn't require platform-specific instructions (except installation)
- [ ] No "works better on Windows/Linux" situations

**Rationale:**
Cross-platform consistency is a core value proposition for PatternCAD.

---

### NFR-012: Maintainability - Code Quality

**Priority:** Must Have

**Description:**
Codebase must be maintainable, well-structured, and documented to enable long-term development by open-source contributors.

**Acceptance Criteria:**
- [ ] Code follows consistent style guide (linting enforced)
- [ ] Functions/methods have single clear responsibility
- [ ] No functions longer than 200 lines (with rare justified exceptions)
- [ ] Complex algorithms include explanatory comments
- [ ] Public APIs have docstrings/documentation comments
- [ ] Code is modular: clear separation of concerns (UI, logic, file I/O, rendering)
- [ ] No copy-paste code: shared logic extracted to functions/classes
- [ ] Technical debt tracked and addressed regularly

**Rationale:**
Open-source project success depends on code that contributors can understand and modify.

---

### NFR-013: Maintainability - Testing Coverage

**Priority:** Should Have

**Description:**
Critical functionality should be covered by automated tests to prevent regressions and enable confident refactoring.

**Acceptance Criteria:**
- [ ] Unit tests for core algorithms: parametric system, constraint solver, nesting
- [ ] Unit tests for file import/export: DXF, SVG, native format
- [ ] Integration tests for key workflows: create, edit, save, load
- [ ] Test coverage target: 60%+ for core modules, 40%+ overall
- [ ] Tests run in CI/CD pipeline on every commit
- [ ] No commits that break existing tests
- [ ] Critical bugs get regression tests added

**Rationale:**
Tests catch bugs early and give contributors confidence to make changes.

---

### NFR-014: Maintainability - Documentation

**Priority:** Must Have

**Description:**
The project must have comprehensive documentation for users and developers to enable adoption and contribution.

**Acceptance Criteria:**
- [ ] User guide covering all major features with screenshots
- [ ] Quick start tutorial: first pattern in 15 minutes
- [ ] Developer documentation: architecture overview, build instructions, contribution guide
- [ ] API documentation for core modules (auto-generated from code)
- [ ] File format specification documented (for future compatibility)
- [ ] Known issues and limitations documented
- [ ] FAQ covering common questions
- [ ] Documentation available in English (additional languages optional)

**Rationale:**
Documentation is critical for user adoption and community contribution.

---

### NFR-015: Compatibility - File Format Standards

**Priority:** Must Have

**Description:**
Imported and exported files must conform to industry standards and work reliably with common CAD tools, plotters, and cutting machines.

**Acceptance Criteria:**
- [ ] DXF export compatible with: AutoCAD, QCAD, LibreCAD, Fusion 360
- [ ] SVG export compatible with: Inkscape, Illustrator, web browsers
- [ ] PDF export compatible with: Adobe Reader, plotting software, print services
- [ ] DXF import handles files from: AutoCAD, Illustrator, Inkscape
- [ ] SVG import handles files from: Inkscape, Illustrator, Figma
- [ ] File format version documented and stable (changes only with major versions)
- [ ] Exported files include metadata: creator app, version, units, scale

**Rationale:**
Interoperability is essential. PatternCAD must integrate into existing workflows.

---

### NFR-016: Compatibility - Hardware Requirements

**Priority:** Must Have

**Description:**
The application must run on modest hardware typical of small workshops and home users, not just high-end workstations.

**Acceptance Criteria:**

**Minimum Requirements:**
- [ ] CPU: Dual-core processor, 2 GHz or faster
- [ ] RAM: 4 GB (runs with acceptable performance)
- [ ] Storage: 200 MB application + user projects
- [ ] Display: 1280×720 resolution minimum
- [ ] Graphics: No dedicated GPU required (software rendering acceptable)
- [ ] OS: Linux (Ubuntu 20.04+, Fedora 35+, Debian 11+, Manjaro), Windows 10/11 64-bit

**Recommended Requirements:**
- [ ] CPU: Quad-core processor, 2.5 GHz or faster
- [ ] RAM: 8 GB (smooth performance for complex projects)
- [ ] Display: 1920×1080 or higher
- [ ] Graphics: Integrated GPU or better (for hardware acceleration)

**Target:** Application runs acceptably on hardware from 2018 or newer.

**Rationale:**
Artisans and small workshops may not have latest hardware. Accessibility requires modest requirements.

---

### NFR-017: Compatibility - Operating System Versions

**Priority:** Must Have

**Description:**
The application must support currently-maintained OS versions and have a clear support policy.

**Acceptance Criteria:**

**Linux Support:**
- [ ] Ubuntu: 20.04 LTS, 22.04 LTS, 24.04 LTS and newer
- [ ] Debian: 11 (Bullseye), 12 (Bookworm) and newer
- [ ] Fedora: Last 3 releases (currently 35+)
- [ ] Manjaro: Current stable release and previous release
- [ ] Arch Linux: Rolling release (best effort)
- [ ] Other distros: Should work if dependencies met (community support)

**Windows Support:**
- [ ] Windows 10: Version 21H2 and newer (64-bit only)
- [ ] Windows 11: All versions (64-bit)
- [ ] Windows 7/8: Not supported

**Support Policy:**
- [ ] Support OS versions that receive security updates from vendor
- [ ] Drop support for EOL OS versions after 6-month grace period
- [ ] Clear documentation of supported OS versions

**Rationale:**
Clear support policy prevents confusion. Focus on currently-maintained OS versions.

---

## Epics

Epics are logical groupings of related functionality that will be broken down into user stories during sprint planning (Phase 4).

Each epic maps to multiple functional requirements and will generate 2-10 stories.

---

### EPIC-001: Core Drawing and Editing Tools

**Description:**
Implement the fundamental vector drawing and editing capabilities that form the foundation of the pattern design system. Users must be able to create basic geometric shapes, bezier curves, and manipulate objects with precision.

**Functional Requirements:**
- FR-001: Draw Basic Geometric Shapes
- FR-002: Draw and Edit Bezier Curves
- FR-003: Select and Edit Vector Objects (including spacebar grab-and-move)

**Story Count Estimate:** 5-8 stories

**Priority:** Must Have

**Business Value:**
This is the absolute foundation of PatternCAD. Without robust drawing tools, no pattern work is possible. These tools must feel professional and responsive to compete with commercial solutions.

**Technical Complexity:** Medium
- Requires solid vector graphics engine
- Mouse/keyboard interaction handling
- Object data model and rendering

---

### EPIC-002: Parametric Design and Constraint System

**Description:**
Build the parametric design system that allows users to create patterns with variables and geometric constraints. This enables professional pattern workflows where dimensions can be adjusted parametrically and relationships are maintained automatically.

**Functional Requirements:**
- FR-004: Define Parametric Dimensions
- FR-005: Apply Geometric Constraints

**Story Count Estimate:** 6-10 stories

**Priority:** Must Have

**Business Value:**
Parametric design is what separates professional CAD tools from simple drawing programs. This is critical for grading, sizing, and creating adaptable patterns that artisans need.

**Technical Complexity:** High
- Requires constraint solver implementation
- Formula parser and evaluator
- Dependency tracking and circular reference detection
- Complex UI for managing parameters and constraints

---

### EPIC-003: Transformation and Manipulation Tools

**Description:**
Implement comprehensive transformation tools allowing users to rotate, mirror, scale, and align pattern pieces. These are essential operations in pattern design workflow.

**Functional Requirements:**
- FR-006: Rotate Objects
- FR-007: Mirror Objects
- FR-008: Scale Objects
- FR-009: Align and Distribute Objects

**Story Count Estimate:** 4-6 stories

**Priority:** Must Have

**Business Value:**
Pattern design requires constant transformation of pieces. Mirror operations are especially critical (left/right pattern pieces). Professional workflow depends on precise, intuitive transformation tools.

**Technical Complexity:** Medium
- Matrix transformations
- Interactive manipulation with visual feedback
- Maintaining parametric relationships during transforms

---

### EPIC-004: Professional Pattern Features

**Description:**
Implement pattern-specific professional features including seam allowance, notches, match points, and size grading. These features distinguish PatternCAD as a pattern design tool rather than a generic CAD application.

**Functional Requirements:**
- FR-010: Add Seam Allowance Automatically
- FR-011: Manage Notches and Match Points
- FR-012: Grade Pattern by Size
- FR-013: Scale Pattern Proportionally

**Story Count Estimate:** 6-9 stories

**Priority:** Must Have

**Business Value:**
These are the domain-specific features that artisans, prototypists, and makers need. Seam allowance and grading are essential for turning designs into manufacturable patterns.

**Technical Complexity:** Medium-High
- Offset curve algorithms (for seam allowance)
- Grading rule system and multi-size generation
- Integration with parametric system

---

### EPIC-005: Layout, Nesting, and Isolation

**Description:**
Provide tools for arranging pattern pieces on material sheets, including manual placement, automatic nesting optimization, and isolation mode for working efficiently with complex projects containing hundreds of pieces.

**Functional Requirements:**
- FR-014: Automatic Nesting of Pattern Pieces
- FR-015: Manual Placement and Arrangement
- FR-027: Isolate Selected Pieces for Performance

**Story Count Estimate:** 5-8 stories

**Priority:** Must Have (FR-015, FR-027) / Should Have (FR-014)

**Business Value:**
Material efficiency directly impacts cost for users. Manual layout is essential (Must Have), automatic nesting provides competitive advantage (Should Have). Isolation mode is critical for projects with hundreds of pieces.

**Technical Complexity:** High
- Nesting algorithms (bin packing, optimization)
- Collision detection and placement validation
- Smart guides and snapping
- Performance optimization for large piece counts
- Isolation mode view management

---

### EPIC-006: Measurement and Dimensioning

**Description:**
Implement precision measurement tools and dimension annotation capabilities for verifying pattern accuracy and creating documentation.

**Functional Requirements:**
- FR-016: Measure Distances and Dimensions
- FR-017: Add Dimension Annotations

**Story Count Estimate:** 3-5 stories

**Priority:** Must Have (FR-016) / Should Have (FR-017)

**Business Value:**
Measurement is critical for pattern accuracy. Professional users must verify dimensions constantly. Annotations are important for documentation and communication but not essential for v1.0.

**Technical Complexity:** Medium
- Accurate measurement algorithms (including curves)
- Associative dimensions that update with geometry
- Dimension rendering and annotation UI

---

### EPIC-007: File Operations and Interoperability

**Description:**
Implement robust file import/export for standard CAD formats (DXF, SVG, PDF), native project file format, and auto-save with recovery. This ensures PatternCAD integrates into existing workflows and protects user data.

**Functional Requirements:**
- FR-018: Import Standard CAD Formats
- FR-019: Export to Standard CAD Formats
- FR-020: Save and Load Native Project Files
- FR-026: Auto-Save with Recovery Files

**Story Count Estimate:** 8-12 stories

**Priority:** Must Have

**Business Value:**
Interoperability is essential for adoption. Users must be able to import existing patterns, export to cutting machines/plotters, and integrate with other tools. Auto-save prevents data loss and builds user trust.

**Technical Complexity:** Medium-High
- Multiple file format parsers/writers (DXF, SVG, PDF)
- Format compatibility testing with many tools
- Native file format design and versioning
- Auto-save background operations and recovery UI

---

### EPIC-008: User Interface and Cross-Platform Foundation

**Description:**
Build the application workspace with multi-panel UI, keyboard shortcuts, layer management, undo/redo system, and ensure full cross-platform compatibility between Linux and Windows.

**Functional Requirements:**
- FR-021: Provide Multi-Panel Workspace
- FR-022: Support Keyboard Shortcuts
- FR-023: Provide Layers and Object Organization
- FR-024: Implement Undo/Redo System
- FR-025: Support Cross-Platform Consistency

**Story Count Estimate:** 8-12 stories

**Priority:** Must Have

**Business Value:**
This is the foundation that all other features build upon. Professional UI with efficient keyboard-driven workflow is essential. Cross-platform support (especially Linux) is a core value proposition and differentiator.

**Technical Complexity:** Medium-High
- Cross-platform GUI framework selection and implementation
- Comprehensive keyboard shortcut system
- Undo/redo architecture (command pattern, memento pattern)
- Layer management system
- Platform-specific testing and optimization
- High-DPI support

---

## User Stories (High-Level)

User stories follow the format: "As a [user type], I want [goal] so that [benefit]."

These are preliminary stories. Detailed stories will be created in Phase 4 (Implementation).

---

Detailed user stories will be created during sprint planning (Phase 4). At that stage, each epic will be broken down into 2-10 actionable stories with:
- Clear user personas
- Specific acceptance criteria
- Story point estimates
- Technical implementation notes
- Dependencies and blockers

---

## User Personas

### Primary Personas

**1. Artisan/Craftsperson**
- **Name:** Marie, Leather Goods Maker
- **Background:** Independent artisan creating custom leather bags, wallets, and accessories
- **Goals:** Create precise patterns for products, grade patterns for different sizes, minimize material waste
- **Frustrations:** Can't afford expensive software, needs Linux support, existing tools too garment-focused
- **Technical Skill:** Moderate (comfortable with computers but not CAD expert)

**2. Prototypist/Product Designer**
- **Name:** Alex, Industrial Designer
- **Background:** Freelance designer creating prototypes for various products requiring fabric/material patterns
- **Goals:** Rapidly iterate on designs, export patterns for fabrication, integrate with other CAD tools
- **Frustrations:** Limited by platform restrictions, needs flexible general-purpose tools not garment-specific
- **Technical Skill:** High (familiar with CAD tools and technical workflows)

**3. Hobbyist/Maker**
- **Name:** Sam, DIY Enthusiast
- **Background:** Hobbyist creating costumes, home goods, and craft projects
- **Goals:** Learn pattern design, create quality projects, explore creative ideas without financial barriers
- **Frustrations:** Commercial software too expensive and complex for occasional use
- **Technical Skill:** Low to Moderate (learning as they go)

### Secondary Personas

**4. Small Business Owner**
- Workshop owner needing affordable professional tools for small team

**5. Educator/Student**
- Teaching or learning design and fabrication in educational setting

---

## User Flows

### Flow 1: Create Simple Pattern from Scratch

1. Launch PatternCAD
2. Create new project
3. Use drawing tools to sketch pattern outline (lines, curves, points)
4. Apply transformations (mirror for left/right symmetry)
5. Add seam allowance
6. Add notches for assembly guidance
7. Measure dimensions to verify accuracy
8. Save project
9. Export to PDF for printing or DXF for cutting machine

**Key Requirements:** FR-001, FR-002, FR-003, FR-006, FR-007, FR-010, FR-011, FR-016, FR-020, FR-019

---

### Flow 2: Import, Modify, and Grade Existing Pattern

1. Import pattern from DXF file (received from client or existing library)
2. Select pattern pieces and make modifications
3. Define parametric dimensions for key measurements
4. Create grading rules for multiple sizes (XS, S, M, L, XL)
5. Generate all size variations
6. Arrange graded sizes on layout sheet
7. Export batch of all sizes to SVG for documentation

**Key Requirements:** FR-018, FR-003, FR-004, FR-012, FR-015, FR-019

---

### Flow 3: Complex Project with Many Pieces

1. Create or import multiple pattern pieces (100+ pieces for complex product)
2. Organize pieces into logical layers (body, lining, pockets, straps, etc.)
3. Work on specific subset using isolation mode (Tab to isolate)
4. Edit isolated pieces without performance impact
5. Exit isolation (Shift+Tab) to see full project
6. Use nesting algorithm to optimize material layout
7. Manually adjust nesting results for grain direction
8. Export final layout to PDF for production

**Key Requirements:** FR-023, FR-027, FR-003, FR-014, FR-015, FR-019

---

## Dependencies

### Internal Dependencies

**UI Framework:**
- All features depend on UI framework (FR-021, EPIC-008)
- Framework must be chosen early: Qt, Electron, GTK, or other cross-platform solution

**Vector Graphics Engine:**
- Drawing, rendering, export all depend on graphics engine
- Candidates: Cairo, Skia, custom OpenGL/Vulkan, or framework-provided

**File I/O System:**
- Native file format must be established before other features can persist data
- Import/export depends on geometry data model

**Undo/Redo System:**
- Must be architected early as all operations depend on it
- Command pattern or similar required

**Cross-Platform Build System:**
- CMake, Meson, or framework-specific build system
- CI/CD pipeline for Linux and Windows builds

---

### External Dependencies

**Standard Libraries:**
- DXF parsing/writing library (e.g., dxflib, libdxfrw)
- SVG parsing/writing (built-in XML parser + SVG spec)
- PDF generation (e.g., libharu, QPdfWriter, or similar)

**Constraint Solver:**
- Geometric constraint solver library or custom implementation
- Candidates: SolveSpace kernel, FreeCAD sketcher, custom solver

**Nesting Algorithm:**
- Bin packing / nesting library or custom implementation
- Candidates: SVGNest, custom genetic algorithm, simple heuristics

**Math Libraries:**
- Linear algebra, geometry operations
- Candidates: Eigen, GLM, or framework-provided

**Platform Integration:**
- File dialogs, system integration (native on each OS)
- Linux: FreeDesktop standards, .desktop files
- Windows: installer (NSIS, Inno Setup, WiX), file associations

---

## Assumptions

1. **User Base Exists:** There is sufficient demand for professional open-source patterning software across artisan, prototypist, and hobbyist communities

2. **Learning Curve Acceptable:** Users are willing to learn CAD-style workflows in exchange for professional-grade results and free access

3. **Community Contribution:** The open-source community will contribute to development, documentation, testing, and support over time

4. **File Format Interoperability:** Standard file formats (DXF, SVG, PDF) provide adequate interoperability with existing tools, plotters, and cutting machines

5. **Cross-Platform Frameworks Sufficient:** Modern cross-platform frameworks (Qt, Electron, GTK) can deliver native-quality user experience on Linux and Windows without significant platform-specific code

6. **Linux User Base:** Linux users represent a meaningful and engaged portion of the target audience, justifying Linux-first development approach

7. **2D Focus Sufficient:** Focusing exclusively on 2D pattern design (no 3D simulation) meets the needs of the target market for v1.0

8. **Modest Hardware Adequate:** Target users have access to modest hardware (2018+ era computers) that can run the application acceptably

9. **No Cloud/Collaboration Required:** Desktop-only application without cloud sync or real-time collaboration features is acceptable for v1.0

10. **Open Source Sustainability:** Project can achieve sustainability through community engagement, potential future donations, or sponsorships without requiring commercial licensing

---

## Out of Scope

### Explicitly Excluded from v1.0

**3D Features:**
- 3D visualization of patterns
- 3D garment simulation or draping
- 3D product rendering

**Pre-Built Content:**
- Template library or pre-made pattern shapes
- Community shape sharing platform
- Built-in pattern wizard or generators

**Cloud and Collaboration:**
- Cloud storage or sync
- Real-time collaboration features
- Multi-user editing
- Online pattern marketplace

**Mobile Platforms:**
- iOS application
- Android application
- Mobile companion apps

**macOS Support:**
- Native macOS application (may be considered for future versions)

**Advanced Rendering:**
- Photorealistic rendering
- Material simulation or physics
- Advanced visualization beyond basic 2D CAD

**Industry-Specific Features:**
- Garment-specific wizards or automation
- Industry-specific compliance tools
- ERP or supply chain integration

**Parametric 3D Modeling:**
- Full parametric 3D CAD capabilities
- Solid modeling

**Hardware Integration (v1.0):**
- Direct cutting machine control
- Plotter drivers (use standard PDF/DXF export instead)
- Hardware calibration tools

---

## Open Questions

**Resolved During Planning:**

All major questions have been addressed during requirements gathering. Implementation details will be resolved during architecture design (Phase 3).

**To Be Resolved in Architecture Phase:**

1. **Technology Stack:** Final selection of GUI framework, graphics engine, and constraint solver
2. **File Format:** Detailed specification of native .patterncad file format (JSON, XML, binary)
3. **Nesting Algorithm:** Choice between existing libraries vs. custom implementation, algorithm complexity
4. **Performance Optimization:** Specific techniques for handling 300+ pieces efficiently
5. **Build System:** CMake vs. Meson vs. framework-specific build system
6. **Distribution:** Linux packaging strategy (AppImage, Flatpak, distro-specific packages)

---

## Approval & Sign-off

### Stakeholders

**Development Team:**
- Core developers and maintainers
- Open-source contributors (community)

**User Community Representatives:**
- Artisan community feedback
- Maker/hobbyist community feedback
- Small fabrication businesses

**Platform Community:**
- Linux desktop community
- Open-source CAD community

### Approval Status

- [ ] Product Owner / Project Lead
- [ ] Technical Lead / Architect
- [ ] Community Representatives
- [ ] Documentation Lead

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-01-27 | Product Manager | Initial PRD created with 27 FRs, 17 NFRs, 8 Epics |

---

## Next Steps

### Phase 3: Architecture

Run `/architecture` to create system architecture based on these requirements.

The architecture will address:
- All functional requirements (FRs)
- All non-functional requirements (NFRs)
- Technical stack decisions
- Data models and APIs
- System components
- Cross-platform implementation strategy

### Phase 4: Sprint Planning

After architecture is complete, run `/sprint-planning` to:
- Break epics into detailed user stories
- Estimate story complexity
- Plan sprint iterations
- Begin implementation

---

**This document was created using BMAD Method v6 - Phase 2 (Planning)**

*To continue: Run `/workflow-status` to see your progress and next recommended workflow.*

---

## Appendix A: Requirements Traceability Matrix

| Epic ID | Epic Name | Functional Requirements | Story Count (Est.) |
|---------|-----------|-------------------------|-------------------|
| EPIC-001 | Core Drawing and Editing Tools | FR-001, FR-002, FR-003 | 5-8 stories |
| EPIC-002 | Parametric Design and Constraint System | FR-004, FR-005 | 6-10 stories |
| EPIC-003 | Transformation and Manipulation Tools | FR-006, FR-007, FR-008, FR-009 | 4-6 stories |
| EPIC-004 | Professional Pattern Features | FR-010, FR-011, FR-012, FR-013 | 6-9 stories |
| EPIC-005 | Layout, Nesting, and Isolation | FR-014, FR-015, FR-027 | 5-8 stories |
| EPIC-006 | Measurement and Dimensioning | FR-016, FR-017 | 3-5 stories |
| EPIC-007 | File Operations and Interoperability | FR-018, FR-019, FR-020, FR-026 | 8-12 stories |
| EPIC-008 | User Interface and Cross-Platform Foundation | FR-021, FR-022, FR-023, FR-024, FR-025 | 8-12 stories |

**Total Estimated Stories: 45-70 stories**

---

## Appendix B: Prioritization Details

### Functional Requirements Priority Summary

**Must Have: 21 FRs**
- FR-001 through FR-008 (Core drawing, parametric, transformations)
- FR-010, FR-011, FR-012, FR-013 (Professional pattern features)
- FR-015, FR-016, FR-018, FR-019, FR-020 (Layout, measurement, file operations)
- FR-021 through FR-027 (UI, keyboard, layers, undo, cross-platform, auto-save, isolation)

**Should Have: 4 FRs**
- FR-009 (Align and distribute)
- FR-014 (Automatic nesting)
- FR-017 (Dimension annotations)

**Could Have: 0 FRs**

**Won't Have (v1.0): 0 FRs**

All planned features are Must Have or Should Have, indicating focused scope on essential functionality.

---

### Non-Functional Requirements Priority Summary

**Must Have: 12 NFRs**
- NFR-001, NFR-002 (Performance)
- NFR-004 (Input validation)
- NFR-005, NFR-006 (Scalability)
- NFR-007, NFR-008 (Reliability)
- NFR-009, NFR-011 (Usability)
- NFR-012, NFR-014 (Maintainability)
- NFR-015, NFR-016, NFR-017 (Compatibility)

**Should Have: 5 NFRs**
- NFR-003 (File protection)
- NFR-010 (Accessibility)
- NFR-013 (Testing coverage)

---

### Epic Priority Summary

**Must Have: 7 Epics**
- EPIC-001: Core Drawing and Editing Tools
- EPIC-002: Parametric Design and Constraint System
- EPIC-003: Transformation and Manipulation Tools
- EPIC-004: Professional Pattern Features
- EPIC-006: Measurement and Dimensioning (FR-016 is Must Have)
- EPIC-007: File Operations and Interoperability
- EPIC-008: User Interface and Cross-Platform Foundation

**Mixed Priority: 1 Epic**
- EPIC-005: Layout, Nesting, and Isolation (FR-015 and FR-027 are Must Have, FR-014 is Should Have)

---

### Development Sequence Recommendation

**Phase 1 - Foundation (16-20 stories):**
- EPIC-008: UI and Cross-Platform Foundation (8-12 stories)
- EPIC-001: Core Drawing and Editing Tools (5-8 stories)

**Phase 2 - Core CAD (10-16 stories):**
- EPIC-003: Transformation and Manipulation Tools (4-6 stories)
- EPIC-002: Parametric Design and Constraint System (6-10 stories)

**Phase 3 - Pattern Features (14-22 stories):**
- EPIC-004: Professional Pattern Features (6-9 stories)
- EPIC-006: Measurement and Dimensioning (3-5 stories)
- EPIC-005: Layout, Nesting, and Isolation (5-8 stories)

**Phase 4 - Integration (8-12 stories):**
- EPIC-007: File Operations and Interoperability (8-12 stories)

**Total Implementation Estimate: 45-70 stories across 4 development phases**

---

**End of Product Requirements Document**
