

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

float imn::ui::font_size() {
    return get_style().font_size;
}
