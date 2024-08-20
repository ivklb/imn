
#include "node_widget.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include <optional>

#include "core/i18n.hpp"
#include "core/object.hpp"
#include "core/setting.hpp"
#include "include/def.hpp"
#include "ui/imgui_helper.hpp"
#include "ui/nodes/nodes.hpp"
#include "util/common.hpp"

using namespace imn::ui;

NodeWidget::NodeWidget() : BaseWidget() {
    // TODO: load from settings
}

NodeWidget::~NodeWidget() {
    // TODO: serialize
    ImNodes::DestroyContext();
}

void NodeWidget::setup() {
    ImNodes::CreateContext();
    ImNodes::GetIO().EmulateThreeButtonMouse.Modifier = &ImGui::GetIO().KeyAlt;
    ImNodes::GetStyle().GridSpacing = get_style().font_size * 2;
}

void NodeWidget::show(ImVec2 size) {
    _show_node_editor();
}

void NodeWidget::process() {
    _graph.process();
}

void NodeWidget::_show_node_editor() {
    ImNodes::BeginNodeEditor();
    // Handle new nodes
    // These are driven by the user, so we place this code before rendering the nodes
    _handle_new_nodes();

    // Draw nodes & links
    for (auto& [nid, node] : _graph.nodes) {
        node->draw_frame();
    }
    for (auto& [lid, link] : _graph.links) {
        ImNodes::Link(link->id, link->start_pid, link->end_pid);
    }

    // Draw minimap
    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
    ImNodes::EndNodeEditor();

    int node_id;
    if (ImNodes::IsNodeHovered(&node_id)) {
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            _graph.nodes[node_id]->on_double_click();
        }
    }

    // Handle new links
    // These are driven by Imnodes, so we place the code after EndNodeEditor().
    _handle_new_links();

    // Handle deleted links
    _handle_deleted_links();
}

void NodeWidget::_handle_new_nodes() {
    const bool open_popup = ImNodes::IsEditorHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right);

    if (!ImGui::IsAnyItemHovered() && open_popup) {
        ImGui::OpenPopup("add node");
    }

    if (ImGui::BeginPopup("add node")) {
        const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
        auto name_list = core::ObjectFactory<Node>::entries();
        for (auto& name : name_list) {
            if (ImGui::MenuItem(I18N_STR(name))) {
                auto node = core::ObjectFactory<Node>::create(name);
                ImNodes::SetNodeScreenSpacePos(node->id, click_pos);
                _graph.insert_node(node);
            }
        }
        ImGui::EndPopup();
    }
}

void NodeWidget::_handle_new_links() {
    int start_pid, end_pid;
    if (ImNodes::IsLinkCreated(&start_pid, &end_pid)) {
        auto start_pin = _graph.get_pin(start_pid, PinKind::Out);
        auto end_pin = _graph.get_pin(end_pid, PinKind::In);

        if (typeid(*start_pin) == typeid(*end_pin)) {
            _graph.insert_link(start_pid, end_pid);
        }
    }
}

void NodeWidget::_handle_deleted_links() {
    {
        int link_id;
        if (ImNodes::IsLinkDestroyed(&link_id)) {
            _graph.erase_link(link_id);
        }
    }

    {
        const int num_selected = ImNodes::NumSelectedLinks();
        if (num_selected > 0 && ImGui::IsKeyReleased(ImGuiKey_Delete)) {
            static std::vector<int> selected_links;
            selected_links.resize(static_cast<size_t>(num_selected));
            ImNodes::GetSelectedLinks(selected_links.data());
            for (const int edge_id : selected_links) {
                _graph.erase_link(edge_id);
            }
        }
    }

    {
        const int num_selected = ImNodes::NumSelectedNodes();
        if (num_selected > 0 && ImGui::IsKeyReleased(ImGuiKey_Delete)) {
            static std::vector<int> selected_nodes;
            selected_nodes.resize(static_cast<size_t>(num_selected));
            ImNodes::GetSelectedNodes(selected_nodes.data());
            for (const int node_id : selected_nodes) {
                _graph.erase_node(node_id);
            }
        }
    }
}
