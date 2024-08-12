
#ifndef UI__STYLE_HPP
#define UI__STYLE_HPP

#include <imgui.h>

namespace imn {
namespace ui {

struct Style {
    float font_size;
    ImVec2 image_button_size;
};

Style& get_style();

}  // namespace ui
}  // namespace imn

#endif
