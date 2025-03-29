cmake_minimum_required(VERSION 3.15)

project(ThreeDEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 3 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(tests)


qt_add_executable(${PROJECT_NAME} main.cpp)
# add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests/Test/Matrix-test.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests)

target_include_directories(${PROJECT_NAME} PUBLIC ${Catch2_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${Qt6_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)

add_subdirectory(MathUtils)
target_link_libraries(${PROJECT_NAME} PRIVATE MathUtils)
add_subdirectory(Object)
target_link_libraries(${PROJECT_NAME} PRIVATE Object)
add_subdirectory(Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Core)

include(CTest)