cmake_minimum_required(VERSION 3.21.0)

if(NOT(DEFINED ENV{CMAKE_TOOLCHAIN_FILE}))
    message(FATAL_ERROR "ERROR: environment variable 'CMAKE_TOOLCHAIN_FILE' not found")
endif()

project(moon)

set(CMAKE_CXX_STANDARD 20)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(gl3w REQUIRED)
find_package(imgui REQUIRED)
find_package(implot REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spdlog REQUIRED)
find_package(VTK REQUIRED)

add_subdirectory(src)
