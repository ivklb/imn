
add_library(dialogs STATIC
    progress_dialog.cpp
)

target_include_directories(dialogs PUBLIC ${SRC_ROOT})

target_link_libraries(dialogs
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
)
