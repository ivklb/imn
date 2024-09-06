
add_executable(${PROJECT_NAME} main.cpp)
add_subdirectory(core)
add_subdirectory(ext/nfd)
add_subdirectory(ext/node_editor)
add_subdirectory(ui)
add_subdirectory(util)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    OpenMP::OpenMP_CXX
    glfw
    imgui::imgui
    implot::implot
    nfd
    opencv_core
    opencv_imgcodecs
    unofficial::gl3w::gl3w
)

target_link_libraries(${PROJECT_NAME}
    core
    ui
    util
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/MP")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/asset $<TARGET_FILE_DIR:${PROJECT_NAME}>/asset
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/python $<TARGET_FILE_DIR:${PROJECT_NAME}>/python
)

if(ENABLE_PYTHON)
    get_filename_component(PY_DIR "${Python_EXECUTABLE}" DIRECTORY)
    message(STATUS "Python executable: ${Python_EXECUTABLE}")
    add_custom_command(TARGET ${PROJECT_NAME}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND}
        -DTARGET_DIR=$<TARGET_FILE_DIR:${PROJECT_NAME}>
        -DPY_DIR=${PY_DIR}
        -P ${CMAKE_SOURCE_DIR}/cmake/install_python.cmake
        COMMENT "installing python..."
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()
