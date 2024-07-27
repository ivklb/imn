
file(GLOB_RECURSE NODE_EDITOR_SRC *.cpp)

add_library(nodes ${NODE_EDITOR_SRC})

target_link_libraries(nodes
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
)

