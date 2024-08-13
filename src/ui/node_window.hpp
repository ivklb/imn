
#ifndef UI__NODE_WINDOW_HPP
#define UI__NODE_WINDOW_HPP

#include <imgui.h>
#include <implot.h>

#include <map>
#include <memory>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <vector>

#include "base_window.hpp"
#include "ext/node_editor/imnodes.h"
#include "ui/nodes/struct.hpp"

namespace imn::ui {

class NodeWindow : public BaseWindow {
   public:
    NodeWindow();
    ~NodeWindow();
    void setup();
    void show() override;

   private:
    void _show_menu_bar();
    void _show_node_editor();
    void _show_info();

    void _handle_new_nodes();
    void _handle_new_links();
    void _handle_deleted_links();

    Graph _graph;

    ImVec2 _mouse_pos;
};

}  // namespace imn::ui

#endif
