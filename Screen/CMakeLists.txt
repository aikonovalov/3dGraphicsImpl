target_sources(ThreeDEngine PRIVATE
    ScreenManager.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# target_link_libraries(Screen
#         Qt6::Core 
#         Qt6::Widgets
#         Qt6::Gui
# )
