
#ifndef UI__NODE_WINDOW_HPP
#define UI__NODE_WINDOW_HPP

#include <imgui.h>
#include <implot.h>

#include <map>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <tuple>
#include <vector>

#include "ext/node_editor/imnodes.h"
#include "ui/base_window.hpp"
#include "ui/nodes/struct.hpp"

namespace imn::ui {

class NodeWidget : public BaseWidget {
   public:
    NodeWidget();
    ~NodeWidget();
    void setup();
    void show(ImVec2 size = ImVec2(0, 0)) override;
    void process();

   private:
    void _show_node_editor();

    void _handle_new_nodes();
    void _handle_new_links();
    void _handle_deleted_links();

    std::shared_ptr<Graph> _graph;

    ImVec2 _mouse_pos;
};

}  // namespace imn::ui

#endif
