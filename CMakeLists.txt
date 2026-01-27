cmake_minimum_required(VERSION 3.16)

project(PatternCAD VERSION 0.1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Svg
    PrintSupport
)

# Find Eigen3 for linear algebra (constraint solver)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# Project Include Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party
)

# Source Files
set(SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/Project.cpp
    src/core/Document.cpp
    src/ui/MainWindow.cpp
    src/ui/Canvas.cpp
    src/ui/ToolPalette.cpp
    src/ui/PropertiesPanel.cpp
    src/ui/LayersPanel.cpp
    src/geometry/GeometryObject.cpp
    src/geometry/Point2D.cpp
    src/geometry/Line.cpp
    src/geometry/Circle.cpp
    src/geometry/Rectangle.cpp
    src/geometry/CubicBezier.cpp
    src/tools/Tool.cpp
    src/tools/SelectTool.cpp
    src/tools/LineTool.cpp
    src/io/FileFormat.cpp
    src/io/NativeFormat.cpp
)

set(HEADERS
    src/core/Application.h
    src/core/Project.h
    src/core/Document.h
    src/ui/MainWindow.h
    src/ui/Canvas.h
    src/ui/ToolPalette.h
    src/ui/PropertiesPanel.h
    src/ui/LayersPanel.h
    src/geometry/GeometryObject.h
    src/geometry/Point2D.h
    src/geometry/Line.h
    src/geometry/Circle.h
    src/geometry/Rectangle.h
    src/geometry/CubicBezier.h
    src/tools/Tool.h
    src/tools/SelectTool.h
    src/tools/LineTool.h
    src/io/FileFormat.h
    src/io/NativeFormat.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Executable
add_executable(PatternCAD
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link Libraries
target_link_libraries(PatternCAD
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::PrintSupport
    Eigen3::Eigen
)

# Compiler Warnings
if(MSVC)
    target_compile_options(PatternCAD PRIVATE /W4)
else()
    target_compile_options(PatternCAD PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installation
install(TARGETS PatternCAD
    RUNTIME DESTINATION bin
)

# Tests (temporarily disabled - will be fixed later)
# enable_testing()
# add_subdirectory(tests)

# Platform-specific settings
if(WIN32)
    set_target_properties(PatternCAD PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
elseif(APPLE)
    set_target_properties(PatternCAD PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist.in
    )
endif()
