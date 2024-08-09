
add_library(common_widgets STATIC
    common_widgets.cpp
)

target_include_directories(common_widgets PUBLIC ${SRC_ROOT})

target_link_libraries(common_widgets
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
)
