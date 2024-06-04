
add_library(ui
    # main_window.cpp
    vtk_viewer.cpp
)

target_link_libraries(ui
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
    ${VTK_LIBRARIES}
)

vtk_module_autoinit(
    TARGETS ui
    MODULES ${VTK_LIBRARIES}
)
