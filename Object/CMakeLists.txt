add_library(stb_image_impl STATIC
    stb_image_impl.cpp
)

target_include_directories(stb_image_impl PUBLIC
    ${CMAKE_SOURCE_DIR}/external/stb
)

add_library(ObjectHeaders INTERFACE)
target_include_directories(ObjectHeaders INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Object
    Camera.cpp
    Object.cpp
    Parser.cpp
)

target_include_directories(Object PRIVATE
    ${CMAKE_SOURCE_DIR}/external/stb
)

target_link_libraries(Object PRIVATE stb_image_impl)
target_link_libraries(Object PRIVATE ObjectHeaders)
target_link_libraries(Object PRIVATE MathUtils)
