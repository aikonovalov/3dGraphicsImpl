set(CMAKE_AUTOMOC OFF)
find_package(Catch2 3 REQUIRED)

add_executable(tests Matrix-test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

include(Catch)