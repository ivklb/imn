
add_library(ui
    image_viewer.cpp
    imgui_helper.cpp
    main_window.cpp
    node_window.cpp
    volume_viewer.cpp
    vtk_viewer.cpp
)
add_subdirectory(dialog)
add_subdirectory(nodes)

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
)

vtk_module_autoinit(
    TARGETS ui
    MODULES ${VTK_LIBRARIES}
)
