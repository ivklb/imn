# set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(util STATIC
    common.cpp
    fs.cpp
)

target_include_directories(util PUBLIC ${SRC_ROOT})

target_link_libraries(util
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
    ${VTK_LIBRARIES}
)
