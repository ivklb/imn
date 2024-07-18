
file(GLOB NODE_EDITOR_SRC *.cpp)

add_library(imgui_node_editor ${NODE_EDITOR_SRC})

target_link_libraries(imgui_node_editor
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
)

