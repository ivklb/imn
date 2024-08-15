
add_library(ui
    imgui_helper.cpp
    main_window.cpp
)
add_subdirectory(dialog)
add_subdirectory(nodes)
add_subdirectory(widgets)

target_include_directories(ui PUBLIC ${SRC_ROOT})

target_link_libraries(ui
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
    ${VTK_LIBRARIES}
    dialogs
    node_editor
    nodes
    widgets
)

vtk_module_autoinit(
    TARGETS ui
    MODULES ${VTK_LIBRARIES}
)
