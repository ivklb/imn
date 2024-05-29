cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(egg)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_VERBOSE_MAKEFILE 1)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(EXEC_NAME egg)

# set(IMGUI_BUILD_GLFW_BINDING TRUE)
# set(IMGUI_BUILD_OPENGL3_BINDING TRUE)

# In this CMakeLists.txt file, components are all built separately and linked together
# If you prefer to compile source files + dependencies (OpenGL loaders, imgui, VtkViewer, etc.)
# together, please see CMakeLists-alt.txt
add_executable(${EXEC_NAME} main.cpp)

find_package(OpenGL REQUIRED)
find_package(gl3w REQUIRED)
find_package(imgui REQUIRED)
# find_package(glfw REQUIRED)
target_link_libraries(${EXEC_NAME} OpenGL::GL)
target_link_libraries(${EXEC_NAME} unofficial::gl3w::gl3w)
target_link_libraries(${EXEC_NAME} glfw)
target_link_libraries(${EXEC_NAME} imgui::imgui)

# VTK
find_package(VTK COMPONENTS
  CommonCore
  CommonColor
  CommonDataModel
  FiltersCore
  InteractionStyle
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
  QUIET
)
if (NOT VTK_FOUND)
  message(FATAL_ERROR "VTK not found!")
  return()
endif()

if (VTK_VERSION VERSION_LESS "9.0.0")
  include(${VTK_USE_FILE})
else()
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${EXEC_NAME}
    MODULES ${VTK_LIBRARIES}
  )
endif()
message (STATUS "VTK Version: ${VTK_VERSION}")

# imgui-vtk (VTK Viewer class)
set(imgui_vtk_viewer_dir ${CMAKE_CURRENT_SOURCE_DIR})
add_library(imgui_vtk_viewer STATIC ${imgui_vtk_viewer_dir}/VtkViewer.cpp)
target_include_directories(imgui_vtk_viewer PUBLIC ${imgui_vtk_viewer_dir})
target_link_libraries(imgui_vtk_viewer unofficial::gl3w::gl3w) # Since gl3w was compiled as a static library, we need to link to it
target_link_libraries(imgui_vtk_viewer imgui::imgui) # Since imgui was compiled as a static library, we need to link to it
target_link_libraries(imgui_vtk_viewer ${VTK_LIBRARIES})
target_link_libraries(${EXEC_NAME} imgui_vtk_viewer)

