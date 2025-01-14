cmake_minimum_required(VERSION 3.15)
project(ThreeDEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(MathUtils)

add_executable(ThreeDEngine main.cpp)

target_link_libraries(ThreeDEngine PRIVATE MathUtilsHeaders)
