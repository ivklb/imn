
#ifndef UI__MAIN_WINDOW_HPP
#define UI__MAIN_WINDOW_HPP

#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include "ui/vtk_viewer.hpp"
#include "ui/imgui_vtk_demo.h"
#include "ui/image_viewer.hpp"
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
    ImageViewer _image_viewer;;

};

#endif
