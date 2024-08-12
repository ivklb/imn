
#ifndef UI__MAIN_WINDOW_HPP
#define UI__MAIN_WINDOW_HPP

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include "core/setting.hpp"
#include "ui/image_viewer.hpp"
#include "ui/imgui_vtk_demo.h"
#include "ui/node_window.hpp"
#include "ui/vtk_viewer.hpp"
#include "util/imgui_util.hpp"

namespace imn {
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
    const char* _glsl_version = "#version 130";
    NodeWindow _node_window;

    std::mutex _mutex_win;
    std::vector<std::shared_ptr<BaseWindow>> _windows;

    std::vector<std::string> _files_to_open;
    bool _load_as_volume;
};

}  // namespace ui
}  // namespace imn
#endif
