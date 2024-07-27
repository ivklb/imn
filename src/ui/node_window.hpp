
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
#include "ui/imgui_node_editor/imgui_node_editor.h"
#include "ui/node.hpp"

namespace ed = ax::NodeEditor;

namespace Moon::ui {

class NodeWindow : public BaseWindow {
   public:
    NodeWindow();
    ~NodeWindow();
    void show() override;

   private:
    std::vector<Node> _nodes;
    std::vector<Link> _links;

    int _next_link_id = 100;

    ed::EditorContext* _context = nullptr;

    ed::NodeId _context_node_id = 0;
    ed::LinkId _context_link_id = 0;
    ed::PinId _context_pin_id = 0;
};

}  // namespace Moon::ui

#endif
