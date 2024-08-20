
#include "pins.hpp"

#include "ext/node_editor/imnodes.h"

using namespace imn::ui;

IntPin::IntPin(Node* node, const char* name, PinKind kind, ColorTheme color)
    : Pin(node, name, kind, color),
      value(0) {
}

void IntPin::draw_frame() {
    const float node_width = node->width();
    const float label_width = ImGui::CalcTextSize(name.c_str()).x;

    ImNodes::PushColorStyle(ImNodesCol_PinHovered, get_highlight_color(color));
    ImNodes::PushColorStyle(ImNodesCol_Pin, get_normal_color(color));
    if (kind == PinKind::In) {
        ImNodes::BeginInputAttribute(id);
        ImGui::TextUnformatted(name.c_str());
        bool connected = node->graph->end_pin_to_link.contains(id);
        if (!connected) {
            ImGui::SameLine();
            auto w = node_width - label_width - ImGui::GetStyle().ItemSpacing.x;
            ImGui::PushItemWidth(w);
            ImGui::DragInt("##hidelabel", &value);
            ImGui::PopItemWidth();
        }
        ImNodes::EndInputAttribute();
    } else if (kind == PinKind::Out) {
        ImNodes::BeginOutputAttribute(id);
        ImGui::Indent(node_width - label_width);
        ImGui::TextUnformatted(name.c_str());
        ImNodes::EndOutputAttribute();
    }

    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

ImagePin::ImagePin(Node* node, const char* name, PinKind kind)
    : Pin(node, name, kind, ColorTheme::Red) {
}

VolumePin::VolumePin(Node* node, const char* name, PinKind kind)
    : Pin(node, name, kind, ColorTheme::Cyan) {
}
