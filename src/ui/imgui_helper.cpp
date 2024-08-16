

#include "imgui_helper.hpp"

#include <imgui.h>

#include <format>
#include <memory>

#include "core/backend.hpp"
#include "util/common.hpp"

namespace imn::ui {
namespace impl {
float _default_font_size() {
    auto [width, height] = get_screen_resolution();
    return height * 0.02f;
}
}  // namespace impl

int IDGenerator::_next_id = 0;
int IDGenerator::next() {
    return _next_id++;
}

void IDGenerator::set_next(int id) {
    _next_id = id;
}

WrapperWindow::WrapperWindow(std::shared_ptr<BaseWidget> widget, const std::string& title, const ImVec2& size)
    : BaseWindow(), _widget(widget), _title(title), _open(true) {
    _id = IDGenerator::next();
}

void WrapperWindow::show(ImVec2 size) {
    if (_open) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Once);

        std::string name = std::format("{}##{}", _title, _id);

        ImGui::Begin(name.c_str(), &_open, ImGuiWindowFlags_NoCollapse);
        _widget->show();
        ImGui::End();
    }
}

Style& get_style() {
    auto size = impl::_default_font_size();
    static Style style = {
        .font_size = size,
        .image_button_size = {size * 2, size * 2},
    };
    return style;
}

float font_size() {
    return get_style().font_size;
}

const ImWchar* GetGlyphRangesGreek() {
    static const ImWchar ranges[] = {
        0x0020,
        0x00FF,  // Basic Latin + Latin Supplement
        0x0370,
        0x03FF,  // Greek and Coptic
        0,
    };
    return &ranges[0];
}

bool ImageButton(
    const char* str_id,
    const std::string& image_file,
    const bool active,
    const ImVec2& image_size,
    const ImVec2& uv0,
    const ImVec2& uv1,
    const ImVec4& bg_col,
    const ImVec4& tint_col) {
    auto tex_id = imn::backend::load_texture_2d(image_file, false);
    if (active) {
        // ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    }
    auto rv = ImGui::ImageButton(str_id, tex_id, image_size, uv0, uv1, bg_col, tint_col);
    if (active) {
        ImGui::PopStyleColor();
    }
    return rv;
}

bool ImageButtonWithText(ImTextureID texture, const std::string& text, float x, float y, float w, float h) {
    ImGui::SetCursorPos({x, y});
    bool clicked = ImGui::ImageButton(("Button_" + text).c_str(), texture, {w, h});
    ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
    ImGui::SetCursorPos({x + (w - text_size.x) * 0.5f, y + (h - text_size.y) * 0.5f});
    ImGui::Text(text.c_str());
    return clicked;
}
}  // namespace imn::ui
