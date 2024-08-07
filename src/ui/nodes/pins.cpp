
#include "pins.hpp"

#include "ui/node_editor/imnodes.h"

using namespace Moon::ui;

IntPin::IntPin(const char* name, PinKind kind, ImColor color) : Pin(name, kind, color) {
}

void IntPin::draw_frame() {
    const float node_width = 100.f;

    ImNodes::PushColorStyle(ImNodesCol_Pin, color);
    if (kind == PinKind::In) {
        ImNodes::BeginInputAttribute(id);
        const float label_width = ImGui::CalcTextSize(name.c_str()).x;
        ImGui::TextUnformatted(name.c_str());
        if (!connected) {
            ImGui::SameLine();
            ImGui::PushItemWidth(node_width - label_width);
            ImGui::DragInt("##hidelabel", &value);
            ImGui::PopItemWidth();
        }
        ImNodes::EndInputAttribute();
    } else if (kind == PinKind::Out) {
        ImNodes::BeginOutputAttribute(id);
        ImGui::TextUnformatted(name.c_str());
        ImNodes::EndOutputAttribute();
    }
    ImNodes::PopColorStyle();
}