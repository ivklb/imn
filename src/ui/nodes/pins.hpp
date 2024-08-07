
#ifndef UI__NODES__PINS_HPP
#define UI__NODES__PINS_HPP

#include "ui/nodes/struct.hpp"
#include <imgui.h>

namespace Moon::ui {

struct IntPin : public Pin {
    int value;

    IntPin(const char* name, PinKind kind, ImColor color = ImColor(255, 255, 255));
    void draw_frame() override ;
};

}  // namespace Moon::UI
#endif
