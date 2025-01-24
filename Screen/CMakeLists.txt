add_library(ScreenHeaders INTERFACE)
target_include_directories(ScreenHeaders INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Screen
    ScreenManager.cpp
)

target_link_libraries(Screen PRIVATE ScreenHeaders)

target_include_directories(Screen PRIVATE ${Qt6Widgets_INCLUDE_DIRS})

target_link_libraries(Screen PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
