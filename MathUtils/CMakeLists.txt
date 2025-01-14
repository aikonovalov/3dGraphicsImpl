cmake_minimum_required(VERSION 3.15)

project(MathUtils LANGUAGES CXX)

add_library(MathUtilsHeaders INTERFACE)

target_include_directories(MathUtilsHeaders INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
