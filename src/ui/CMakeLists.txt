
add_library(ui
    image_viewer.cpp
    main_window.cpp
    node_window.cpp
    style.cpp
    vtk_viewer.cpp
)
add_subdirectory(widget)
add_subdirectory(dialog)
add_subdirectory(nodes)
add_subdirectory(imgui_node_editor)

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
    nodes
)

vtk_module_autoinit(
    TARGETS ui
    MODULES ${VTK_LIBRARIES}
)
