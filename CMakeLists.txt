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
    Xml
)

# Find Eigen3 for linear algebra (constraint solver)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# Project Include Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/Clipper2/CPP/Clipper2Lib/include
)

# Clipper2 Sources
set(CLIPPER2_SOURCES
    third-party/Clipper2/CPP/Clipper2Lib/src/clipper.engine.cpp
    third-party/Clipper2/CPP/Clipper2Lib/src/clipper.offset.cpp
)

# Source Files
set(SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/Project.cpp
    src/core/Document.cpp
    src/core/Commands.cpp
    src/core/Units.cpp
    src/core/SettingsManager.cpp
    src/core/AutoSaveManager.cpp
    src/ui/MainWindow.cpp
    src/ui/Canvas.cpp
    src/ui/DimensionRenderer.cpp
    src/ui/DimensionInputWidget.cpp
    src/ui/DimensionInputOverlay.cpp
    src/ui/ToolPalette.cpp
    src/ui/PropertiesPanel.cpp
    src/ui/LayersPanel.cpp
    src/ui/ParametersPanel.cpp
    src/ui/ObjectsPanel.cpp
    src/ui/KeyboardShortcutsDialog.cpp
    src/ui/PreferencesDialog.cpp
    src/ui/RecoveryDialog.cpp
    src/ui/SeamAllowanceWidget.cpp
    src/geometry/GeometryObject.cpp
    src/geometry/Point2D.cpp
    src/geometry/Line.cpp
    src/geometry/Circle.cpp
    src/geometry/Rectangle.cpp
    src/geometry/Polyline.cpp
    src/geometry/CubicBezier.cpp
    src/geometry/SeamAllowance.cpp
    src/geometry/Notch.cpp
    src/geometry/MatchPoint.cpp
    src/geometry/GradingSystem.cpp
    src/tools/Tool.cpp
    src/tools/SelectTool.cpp
    src/tools/PolylineTool.cpp
    src/tools/AddPointOnContourTool.cpp
    src/tools/RotateTool.cpp
    src/tools/MirrorTool.cpp
    src/tools/ScaleTool.cpp
    src/tools/SeamAllowanceTool.cpp
    src/tools/NotchTool.cpp
    src/tools/MatchPointTool.cpp
    src/io/FileFormat.cpp
    src/io/NativeFormat.cpp
    src/io/SVGFormat.cpp
    src/io/PDFFormat.cpp
    src/io/DXFFormat.cpp
    src/ui/GradingDialog.cpp
    src/ui/ScalePatternDialog.cpp
    src/core/GradingCommands.cpp
)

set(HEADERS
    src/core/Application.h
    src/core/Project.h
    src/core/Document.h
    src/core/Commands.h
    src/core/Units.h
    src/core/SettingsManager.h
    src/core/AutoSaveManager.h
    src/ui/MainWindow.h
    src/ui/Canvas.h
    src/ui/DimensionRenderer.h
    src/ui/DimensionInputWidget.h
    src/ui/DimensionInputOverlay.h
    src/ui/ToolPalette.h
    src/ui/PropertiesPanel.h
    src/ui/LayersPanel.h
    src/ui/ParametersPanel.h
    src/ui/ObjectsPanel.h
    src/ui/KeyboardShortcutsDialog.h
    src/ui/PreferencesDialog.h
    src/ui/RecoveryDialog.h
    src/ui/SeamAllowanceWidget.h
    src/geometry/GeometryObject.h
    src/geometry/Point2D.h
    src/geometry/Line.h
    src/geometry/Circle.h
    src/geometry/Rectangle.h
    src/geometry/Polyline.h
    src/geometry/CubicBezier.h
    src/geometry/SeamAllowance.h
    src/geometry/Notch.h
    src/geometry/MatchPoint.h
    src/tools/Tool.h
    src/tools/SelectTool.h
    src/tools/PolylineTool.h
    src/tools/AddPointOnContourTool.h
    src/tools/RotateTool.h
    src/tools/MirrorTool.h
    src/tools/ScaleTool.h
    src/tools/SeamAllowanceTool.h
    src/tools/NotchTool.h
    src/tools/MatchPointTool.h
    src/io/FileFormat.h
    src/io/NativeFormat.h
    src/io/SVGFormat.h
    src/io/PDFFormat.h
    src/io/DXFFormat.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Executable
add_executable(PatternCAD
    ${CLIPPER2_SOURCES}
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
    Qt6::Xml
    Eigen3::Eigen
)

# Compiler Warnings
if(MSVC)
    target_compile_options(PatternCAD PRIVATE /W4)
else()
    target_compile_options(PatternCAD PRIVATE -Wall -Wextra -Wpedantic)
endif()

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

# Installation
include(GNUInstallDirs)

install(TARGETS PatternCAD
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)

# Install desktop entry (Linux)
if(UNIX AND NOT APPLE)
    install(FILES resources/patterncad.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )

    # Install MIME type
    install(FILES resources/patterncad.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/mime/packages
    )

    # Install icon (if exists)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/patterncad.png)
        install(FILES resources/icons/patterncad.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps
        )
    endif()
endif()

# Tests (temporarily disabled - will be fixed later)
# enable_testing()
# add_subdirectory(tests)

# Packaging
include(cmake/Packaging.cmake)
