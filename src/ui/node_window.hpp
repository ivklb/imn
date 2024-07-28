
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
#include "ui/nodes/struct.hpp"

namespace ed = ax::NodeEditor;

namespace Moon::ui {

class NodeWindow : public BaseWindow {
   public:
    NodeWindow();
    ~NodeWindow();
    void show() override;

   private:
    Node* _find_node(ed::NodeId id);
    Link* _find_link(ed::LinkId id);
    Pin* _find_pin(ed::PinId id);

    std::map<ed::NodeId, Node> _nodes;
    std::map<ed::LinkId, Link> _links;

    ImVec2 _mouse_pos;
    ed::EditorContext* _context = nullptr;
    ed::NodeId _context_node_id = 0;
    ed::LinkId _context_link_id = 0;
    ed::PinId _context_pin_id = 0;
};

}  // namespace Moon::ui

#endif
