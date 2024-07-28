
#include "node_window.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include <optional>

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

    auto tmp_node1 = create_simple_node();
    auto tmp_node2 = create_simple_node();
    _nodes[tmp_node1.id] = tmp_node1;
    _nodes[tmp_node2.id] = tmp_node2;
}

NodeWindow::~NodeWindow() {
    ed::DestroyEditor(_context);
}

void NodeWindow::show() {
    ed::SetCurrentEditor(_context);
    ed::Begin("##MyEditor", ImVec2(0.0, 0.0f));

    for (auto& [nid, n] : _nodes) {
        n.on_frame();
    }
    for (auto& [lid, link] : _links) {
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
                    _links[link.id] = link;
                    _find_pin(inputPinId)->connected = true;
                    _find_pin(outputPinId)->connected = true;
                    ed::Link(link.id, link.start_pid, link.end_pid);
                }

                // You may choose to reject connection between these nodes
                // by calling ed::RejectNewItem(). This will allow editor to give
                // visual feedback by changing link thickness and color.
            }
        }
        // ed::PinId pinId = 0;
        // if (ed::QueryNewNode(&pinId)) {
        //     if (ed::AcceptNewItem()) {
        //         // createNewNode = true;
        //         // newNodeLinkPin = FindPin(pinId);
        //         // newLinkPin = nullptr;
        //         ed::Suspend();
        //         // ImGui::OpenPopup("Create New Node");
        //         ed::Resume();
        //     }
        // }
    }
    ed::EndCreate();  // Wraps up object creation action handling.

    // Handle deletion action
    if (ed::BeginDelete()) {
        ed::NodeId nodeId = 0;
        while (ed::QueryDeletedNode(&nodeId)) {
            if (ed::AcceptDeletedItem()) {
                if (_nodes.contains(nodeId)) {
                    _nodes.erase(nodeId);
                    // TODO: delete link
                    break;
                }
            }
        }

        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId)) {
            if (ed::AcceptDeletedItem()) {
                if (_links.contains(deletedLinkId)) {
                    _links.erase(deletedLinkId);
                    break;
                }
            }
        }
    }
    ed::EndDelete();  // Wrap up deletion action

    auto tmp_mouse_pos = ImGui::GetMousePos();
    ed::Suspend();
    if (ed::ShowNodeContextMenu(&_context_node_id)) {
        ImGui::OpenPopup("Node Context Menu");
    } else if (ed::ShowPinContextMenu(&_context_pin_id)) {
        ImGui::OpenPopup("Pin Context Menu");
    } else if (ed::ShowLinkContextMenu(&_context_link_id)) {
        ImGui::OpenPopup("Link Context Menu");
    } else if (ed::ShowBackgroundContextMenu()) {
        _mouse_pos = tmp_mouse_pos;
        ImGui::OpenPopup("Create New Node");
    }
    ed::Resume();

    ed::Suspend();
    if (ImGui::BeginPopup("Create New Node")) {
        std::optional<Node> node;
        if (ImGui::MenuItem("dummy")) {
        }
        if (ImGui::BeginMenu("New Node")) {
            if (ImGui::MenuItem("Simple Node")) {
                node = create_simple_node();
            }
            ImGui::EndMenu();
        }
        if (node.has_value()) {
            _nodes[node.value().id] = node.value();
            ed::SetNodePosition(node.value().id, _mouse_pos);
        }

        ImGui::EndPopup();
    }
    ed::Resume();

    ed::End();
    ed::SetCurrentEditor(nullptr);
}

Node* NodeWindow::_find_node(ed::NodeId id) {
    if (_nodes.contains(id)) {
        return &_nodes[id];
    }
    return nullptr;
}

Link* NodeWindow::_find_link(ed::LinkId id) {
    if (_links.contains(id)) {
        return &_links[id];
    }
    return nullptr;
}

Pin* NodeWindow::_find_pin(ed::PinId id) {
    for (auto& [nid, n] : _nodes) {
        for (auto& p : n.inputs) {
            if (p.id == id) {
                return &p;
            }
        }
        for (auto& p : n.outputs) {
            if (p.id == id) {
                return &p;
            }
        }
    }
    return nullptr;
}
