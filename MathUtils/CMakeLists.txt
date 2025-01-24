add_library(MathUtilsHeaders INTERFACE)
target_include_directories(MathUtilsHeaders INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(MathUtils
    Point3d.cpp
    Plane.cpp
    Triangle.cpp
)
target_link_libraries(MathUtils PRIVATE MathUtilsHeaders)
