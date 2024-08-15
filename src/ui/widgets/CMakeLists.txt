
set(LIBNAME widgets)

add_library(
    ${LIBNAME}
    image_widget.cpp
    node_widget.cpp
    volume_widget.cpp
    vtk_viewer.cpp
)

target_include_directories(${LIBNAME} PUBLIC ${SRC_ROOT})

target_link_libraries(${LIBNAME}
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
    utf8::cpp
    utf8cpp::utf8cpp
    ${VTK_LIBRARIES}
)
