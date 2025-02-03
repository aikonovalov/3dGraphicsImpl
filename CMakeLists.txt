cmake_minimum_required(VERSION 3.15)
project(ThreeDEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/Qt/lib/cmake")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_subdirectory(MathUtils)
add_subdirectory(Object)
add_subdirectory(Screen)

add_executable(ThreeDEngine main.cpp)

target_link_libraries(ThreeDEngine PRIVATE MathUtils)
target_link_libraries(ThreeDEngine PRIVATE Object)
target_link_libraries(ThreeDEngine PRIVATE Screen Qt6::Core Qt6::Gui Qt6::Widgets)

# option(ENABLE_SANITIZERS "Enable sanitizers for debugging" OFF)

# if (ENABLE_SANITIZERS)
#     target_compile_options(ThreeDEngine PRIVATE -fsanitize=address)
#     target_link_options(ThreeDEngine PRIVATE -fsanitize=address)
#     message(STATUS "Sanitizers enabled: -fsanitize=address")
# else()
#     message(STATUS "Sanitizers are disabled.")
# endif()
