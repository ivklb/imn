
#ifndef UI__NODES__PINS_HPP
#define UI__NODES__PINS_HPP

#include <imgui.h>

#include <memory>
#include <opencv2/opencv.hpp>

#include "ui/nodes/struct.hpp"

namespace Moon::ui {

struct IntPin : public Pin {
    int value;

    IntPin(const char* name, PinKind kind, ColorTheme color = ColorTheme::Blue);
    void draw_frame() override;
};

struct ImagePin : public Pin {
    ImagePin(const char* name, PinKind kind);
};

}  // namespace Moon::ui
#endif
