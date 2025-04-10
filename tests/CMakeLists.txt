set(CMAKE_AUTOMOC OFF)
find_package(Catch2 3 REQUIRED)

add_executable(tests Clipping-test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE MathUtils)

include(Catch)