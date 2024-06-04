
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
#include "core/moon.hpp"


class MainWindow {
public:
    void show();

};

#endif
