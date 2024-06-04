
#ifndef UI__MAIN_WINDOW_HPP
#define UI__MAIN_WINDOW_HPP

// Standard Library
#include <iostream>

#include "ui/vtk_viewer.hpp"
#include <vtkSmartPointer.h>
#include <vtkActor.h>
// OpenGL Loader
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// ImGui + imgui-vtk
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

// File-Specific Includes
#include "ui/imgui_vtk_demo.h" // Actor generator for this demo
#include "util/imgui_util.hpp"
#include "core/setting.hpp"


class MainWindow {
public:
    MainWindow();

    void show();
private:
    void _setup();
    void _setup_gl();
    void _setup_imgui();
    void _create_dock_space_and_menubar();
    void _cleanup();

private:
    GLFWwindow* _window;
    const char* _glsl_version = "#version 130";
    GLuint _out_texture;

};

#endif
