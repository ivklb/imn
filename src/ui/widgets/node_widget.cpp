
#include "node_widget.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include <optional>

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
        if (ImGui::MenuItem("add")) {
            auto node = std::make_shared<DemoNode>("add");
            _graph.insert_node(node);
            ImNodes::SetNodeScreenSpacePos(node->id, click_pos);
        }
        if (ImGui::MenuItem("multiply")) {
            auto node = std::make_shared<DemoNode>("multiply");
            _graph.insert_node(node);
            ImNodes::SetNodeScreenSpacePos(node->id, click_pos);
        }
        if (ImGui::MenuItem("image loader")) {
            auto node = std::make_shared<ImageLoaderNode>();
            _graph.insert_node(node);
            ImNodes::SetNodeScreenSpacePos(node->id, click_pos);
        }
        if (ImGui::MenuItem("volume loader")) {
            auto node = std::make_shared<VolumeLoaderNode>();
            _graph.insert_node(node);
            ImNodes::SetNodeScreenSpacePos(node->id, click_pos);
        }
        if (ImGui::MenuItem("image preview")) {
            auto node = std::make_shared<ImagePreviewNode>();
            _graph.insert_node(node);
            ImNodes::SetNodeScreenSpacePos(node->id, click_pos);
        }
        if (ImGui::MenuItem("volume preview")) {
            auto node = std::make_shared<VolumePreviewNode>();
            _graph.insert_node(node);
            ImNodes::SetNodeScreenSpacePos(node->id, click_pos);
        }
        ImGui::EndPopup();
    }
}

void NodeWidget::_handle_new_links() {
    int start_attr, end_attr;
    if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) {
        auto input_pin = _graph.pin(start_attr);
        auto output_pin = _graph.pin(end_attr);

        // TODO: check for valid link

        // const bool valid_link = start_type != end_type;
        // if (valid_link) {
        //     // Ensure the edge is always directed from the value to
        //     // whatever produces the value
        //     if (start_type != NodeType::value) {
        //         std::swap(start_attr, end_attr);
        //     }
        // }
        _graph.insert_link(start_attr, end_attr);
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
