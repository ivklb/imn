
#ifndef UI__NODES__PINS_HPP
#define UI__NODES__PINS_HPP

#include <imgui.h>

#include "ui/nodes/struct.hpp"

namespace Moon::ui {

struct IntPin : public Pin {
    int value;

    IntPin(const char* name, PinKind kind, ColorTheme color = ColorTheme::Blue);
    void draw_frame() override;
};

}  // namespace Moon::ui
#endif
