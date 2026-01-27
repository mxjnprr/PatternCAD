# PatternCAD Tests

# Find Qt Test module
find_package(Qt6 REQUIRED COMPONENTS Test)

# Test executable
set(TEST_SOURCES
    test_geometry.cpp
    test_constraints.cpp
    test_fileio.cpp
)

add_executable(PatternCADTests
    ${TEST_SOURCES}
)

target_link_libraries(PatternCADTests
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Eigen3::Eigen
)

# Add tests to CTest
add_test(NAME GeometryTests COMMAND PatternCADTests)
