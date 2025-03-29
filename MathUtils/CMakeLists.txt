add_library(MathUtilsHeaders INTERFACE)
target_include_directories(MathUtilsHeaders INTERFACE ${CMAKE_SOURCE_DIR})

add_library(MathUtils
    Point4.cpp
    Triangle.cpp
    Plane.cpp
)
target_link_libraries(MathUtils PRIVATE MathUtilsHeaders)
