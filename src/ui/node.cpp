
#include "node.hpp"

#include <vector>

#include "ui/imgui_node_editor/imgui_node_editor.h"
#include "ui/style.hpp"
#include "util/common.hpp"

namespace ed = ax::NodeEditor;
using namespace ax;
using ax::Widgets::IconType;
using namespace Moon::ui;

Pin::Pin(const char* name, ed::PinKind kind, ax::Widgets::IconType type, ImColor color)
    : node(nullptr),
      name(name),
      kind(kind),
      type(type),
      color(color),
      connected(false) {
    id = get_unique_id();
}

void Pin::on_frame() {
    ed::BeginPin(id, kind);

    int alpha = 255;
    float icon_size = ui::get_style().font_size;

    if (kind == ed::PinKind::Input) {
        ax::Widgets::Icon(ImVec2(icon_size, icon_size), type, connected, color, ImColor(32, 32, 32, alpha));
        ImGui::SameLine();
        ImGui::Text(name.c_str());
    } else if (kind == ed::PinKind::Output) {
        ImGui::Text(name.c_str());
        ImGui::SameLine();
        ax::Widgets::Icon(ImVec2(icon_size, icon_size), type, connected, color, ImColor(32, 32, 32, alpha));
    }
    ed::EndPin();
}

void Node::on_frame() {
    ed::BeginNode(id);

    ImGui::BeginGroup();
    ImGui::Text(name.c_str());

    auto padding2 = ed::GetStyle().NodePadding;
    ImGui::Dummy(ImVec2(0.0f, padding2.y / 2));
    ImGui::EndGroup();
    auto header_top_left = ImGui::GetItemRectMin();
    auto header_bottom_right = ImGui::GetItemRectMax();

    for (auto& pin : inputs) {
        pin.on_frame();
    }
    ImGui::SameLine();
    for (auto& pin : outputs) {
        pin.on_frame();
    }

    ed::EndNode();
    auto node_bottom_right = ImGui::GetItemRectMax();

    auto drawList = ed::GetNodeBackgroundDrawList(id);

    auto padding = ed::GetStyle().NodePadding;
    const auto border_width = ed::GetStyle().NodeBorderWidth;

    // draw header background
    drawList->AddRectFilled(
        header_top_left - ImVec2(padding.x - border_width, padding.y - border_width),
        ImVec2(node_bottom_right.x - border_width, header_bottom_right.y),
        color,
        ed::GetStyle().NodeRounding,
        ImDrawFlags_RoundCornersTop);

    if (ImGui::IsItemHovered()) {
        ed::Suspend();
        ImGui::SetTooltip("State: %s", "123");
        ed::Resume();
    }
}
