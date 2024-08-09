

#include "style.hpp"

#include <imgui.h>

#include "util/common.hpp"

namespace {

float _default_font_size() {
    auto [width, height] = get_screen_resolution();
    return height * 0.02f;
}
}  // namespace

imn::ui::Style& imn::ui::get_style() {
    static Style style = {
        .font_size = _default_font_size(),
        .image_button_size = {_default_font_size() * 2, _default_font_size() * 2},
    };
    return style;
}

float imn::ui::get_input_box_height() {
    // https://github.com/ocornut/imgui/issues/4511#issuecomment-913381339
    return get_style().font_size + ImGui::GetStyle().FramePadding.y * 2;
}
