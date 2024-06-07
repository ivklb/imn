
add_library(ui
    main_window.cpp
    vtk_viewer.cpp
)
add_subdirectory(widget)

target_include_directories(ui PUBLIC ${SRC_ROOT})

target_link_libraries(ui
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
    ${VTK_LIBRARIES}
    common_widgets
)

vtk_module_autoinit(
    TARGETS ui
    MODULES ${VTK_LIBRARIES}
)
