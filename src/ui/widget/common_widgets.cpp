
#include "ui/widget/common_widgets.hpp"

#include "util/imgui_util.hpp"

bool ImageButton(
    const char* str_id,
    const std::string& image_file,
    const bool active,
    const ImVec2& image_size,
    const ImVec2& uv0,
    const ImVec2& uv1,
    const ImVec4& bg_col,
    const ImVec4& tint_col
) {
    auto tex_id = load_texture_2d(image_file, false);
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
    ImGui::SetCursorPos({ x, y });
    bool clicked = ImGui::ImageButton(("Button_" + text).c_str(), texture, { w, h });
    ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
    ImGui::SetCursorPos({ x + (w - text_size.x) * 0.5f, y + (h - text_size.y) * 0.5f });
    ImGui::Text(text.c_str());
    return clicked;
}

