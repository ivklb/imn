cmake_minimum_required(VERSION 3.21.0)

if(NOT(DEFINED ENV{CMAKE_TOOLCHAIN_FILE}))
    message(FATAL_ERROR "ERROR: environment variable 'CMAKE_TOOLCHAIN_FILE' not found")
endif()

project(imn)

set(CMAKE_CXX_STANDARD 20)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(gl3w REQUIRED)
find_package(imgui REQUIRED)
find_package(implot REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED) # should find python first, then pybind11, link library pybind11::embed
find_package(spdlog REQUIRED)
find_package(utf8cpp REQUIRED)
find_package(VTK REQUIRED)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)
# disable glog in folly libaray
# add_definitions(-DGLOG_USE_GLOG_EXPORT)
# add_definitions(-DGLOG_NO_ABBREVIATED_SEVERITIES)

include(${CMAKE_SOURCE_DIR}/cmake/config.cmake)
add_subdirectory(src)
