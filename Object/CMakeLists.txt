add_library(ObjectHeaders INTERFACE)
target_include_directories(ObjectHeaders INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Object
    Camera.cpp
    Object.cpp
    Holder.cpp
)

target_link_libraries(Object PRIVATE ObjectHeaders)
