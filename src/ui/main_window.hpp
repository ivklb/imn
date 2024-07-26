
#ifndef UI__MAIN_WINDOW_HPP
#define UI__MAIN_WINDOW_HPP

// TODO: please pay attention to the order of vtk & OpenGL headers
#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <memory>
#include <optional>
#include <vector>

#include "ui/vtk_viewer.hpp"
#include "ui/imgui_vtk_demo.h"
#include "ui/image_viewer.hpp"
#include "ui/node_window.hpp"
#include "util/imgui_util.hpp"
#include "core/setting.hpp"


namespace Moon {
namespace ui {


class MainWindow {
public:
    MainWindow();

    void show();
private:
    void _setup();
    void _setup_gl();
    void _setup_imgui();
    void _on_frame();
    void _create_dock_space_and_menubar();
    void _show_dialog();
    void _cleanup();

private:
    GLFWwindow* _window;
    const char* _glsl_version = "#version 130";
    ImageViewer _image_viewer;
    NodeWindow _node_window;
    std::vector<std::shared_ptr<BaseWindow>> _windows;

    std::vector<std::string> _files_to_open;
    bool _load_as_volume;
};

}
}
#endif
