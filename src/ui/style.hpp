
#ifndef UI__STYLE_HPP
#define UI__STYLE_HPP

#include <imgui.h>


namespace Moon {
namespace ui {

struct Style {
    float font_size;
    ImVec2 image_button_size;
};

Style& get_style();

float get_input_box_height();

}
}


#endif
