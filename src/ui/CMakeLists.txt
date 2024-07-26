
add_library(ui
    main_window.cpp
    image_viewer.cpp
    node_window.cpp
    style.cpp
    vtk_viewer.cpp
)
add_subdirectory(widget)
add_subdirectory(dialog)
add_subdirectory(node_editor)

target_include_directories(ui PUBLIC ${SRC_ROOT})

target_link_libraries(ui
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
    ${VTK_LIBRARIES}
    common_widgets
    dialogs
    imgui_node_editor
)

vtk_module_autoinit(
    TARGETS ui
    MODULES ${VTK_LIBRARIES}
)
