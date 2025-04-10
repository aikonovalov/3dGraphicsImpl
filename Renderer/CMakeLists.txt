add_library(RendererHeaders INTERFACE)
target_include_directories(RendererHeaders INTERFACE $(CMAKE_CURRENT_SOURCE_DIR))

add_library(Renderer
    Renderer.cpp
)

target_link_libraries(Renderer PRIVATE RendererHeaders)
target_link_libraries(Renderer PRIVATE Object)