
#include "node_window.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include "core/setting.hpp"
#include "include/def.hpp"
#include "ui/imgui_node_editor/utilities/widgets.h"
#include "ui/nodes/demo.hpp"
#include "ui/style.hpp"
#include "ui/widget/common_widgets.hpp"
#include "util/common.hpp"
#include "util/imgui_util.hpp"

using namespace Moon::ui;
namespace ed = ax::NodeEditor;

NodeWindow::NodeWindow() {
    ed::Config config;
    config.SettingsFile = "Simple.json";
    _context = ed::CreateEditor(&config);
    _nodes.push_back(create_simple_node());
}

NodeWindow::~NodeWindow() {
    ed::DestroyEditor(_context);
}

void NodeWindow::show() {
    ed::SetCurrentEditor(_context);
    ed::Begin("##MyEditor", ImVec2(0.0, 0.0f));

    for (auto& n : _nodes) {
        n.on_frame();
    }
    for (auto& link : _links) {
        ed::Link(link.id, link.start_pid, link.end_pid);
    }

    if (ed::BeginCreate()) {
        ed::PinId inputPinId, outputPinId;
        if (ed::QueryNewLink(&inputPinId, &outputPinId)) {
            // QueryNewLink returns true if editor want to create new link between pins.
            //
            // Link can be created only for two valid pins, it is up to you to
            // validate if connection make sense. Editor is happy to make any.
            //
            // Link always goes from input to output. User may choose to drag
            // link from output pin or input pin. This determine which pin ids
            // are valid and which are not:
            //   * input valid, output invalid - user started to drag new ling from input pin
            //   * input invalid, output valid - user started to drag new ling from output pin
            //   * input valid, output valid   - user dragged link over other pin, can be validated

            if (inputPinId && outputPinId) {
                // ed::AcceptNewItem() return true when user release mouse button.
                if (ed::AcceptNewItem()) {
                    Link link{inputPinId, outputPinId};
                    _links.push_back(link);
                    ed::Link(link.id, link.start_pid, link.end_pid);
                }

                // You may choose to reject connection between these nodes
                // by calling ed::RejectNewItem(). This will allow editor to give
                // visual feedback by changing link thickness and color.
            }
        }
    }
    ed::EndCreate();  // Wraps up object creation action handling.

    // Handle deletion action
    if (ed::BeginDelete()) {
        ed::NodeId nodeId = 0;
        while (ed::QueryDeletedNode(&nodeId)) {
            if (ed::AcceptDeletedItem()) {
                auto id = std::find_if(_nodes.begin(), _nodes.end(), [nodeId](auto& node) { return node.id == nodeId; });
                if (id != _nodes.end()) {
                    _nodes.erase(id);
                    // TODO: delete link
                    break;
                }
            }
        }

        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId)) {
            if (ed::AcceptDeletedItem()) {
                auto id = std::find_if(_links.begin(), _links.end(), [=](auto& link) { return link.id == deletedLinkId; });
                if (id != _links.end()) {
                    _links.erase(id);
                    break;
                }
            }
        }
    }
    ed::EndDelete();  // Wrap up deletion action

    ed::End();
    ed::SetCurrentEditor(nullptr);
}
