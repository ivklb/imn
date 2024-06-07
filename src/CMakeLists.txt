
add_executable(${PROJECT_NAME} main.cpp)
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(util)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    unofficial::gl3w::gl3w
    glfw
    imgui::imgui
    implot::implot
    OpenMP::OpenMP_CXX
    opencv_core
    opencv_imgcodecs
)

target_link_libraries(${PROJECT_NAME} core)
target_link_libraries(${PROJECT_NAME} ui)
target_link_libraries(${PROJECT_NAME} util)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/MP")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/asset
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/asset)
