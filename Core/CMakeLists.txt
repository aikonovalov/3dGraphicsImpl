add_library(CoreHeaders INTERFACE)
target_include_directories(CoreHeaders INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Core
    Model.cpp
    View.cpp
    Controller.cpp
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

target_link_libraries(Core PRIVATE CoreHeaders)

target_include_directories(Core PUBLIC ${Qt6_INCLUDE_DIRS})
target_link_libraries(Core PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)
