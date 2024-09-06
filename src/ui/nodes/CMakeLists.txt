
add_library(
    nodes
    nodes.cpp
    pins.cpp
    python_nodes.cpp
    struct.cpp
)

message(STATUS "================================")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "================================")
target_include_directories(nodes PUBLIC ${SRC_ROOT})

target_link_libraries(nodes
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
    utf8::cpp
    utf8cpp::utf8cpp
    ${VTK_LIBRARIES}
)
