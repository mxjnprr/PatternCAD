# Third-Party Dependencies

This directory contains third-party libraries and dependencies.

## Required Dependencies

### Eigen3
- **Purpose**: Linear algebra library for constraint solver
- **Version**: 3.3+
- **Installation**:
  - Ubuntu/Debian: `sudo apt-get install libeigen3-dev`
  - macOS: `brew install eigen`
  - Windows: Download from http://eigen.tuxfamily.org/

### Qt6
- **Purpose**: GUI framework
- **Version**: 6.2+
- **Modules**: Core, Gui, Widgets, Svg, PrintSupport
- **Installation**: Download from https://www.qt.io/download-open-source

## Optional Dependencies

### Clipper2
- **Purpose**: Polygon offsetting for seam allowances
- **Version**: Latest
- **Repository**: https://github.com/AngusJohnson/Clipper2

### libdxfrw
- **Purpose**: DXF file format import/export
- **Repository**: https://github.com/LibreCAD/libdxfrw

## Integration

Most dependencies are found via CMake's `find_package()`. Some may need to be included as git submodules or copied here directly.

To add a git submodule:
```bash
git submodule add <repository-url> third-party/<library-name>
```
