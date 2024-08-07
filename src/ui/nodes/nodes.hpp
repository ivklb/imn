
#ifndef UI__NODES__NODES_HPP
#define UI__NODES__NODES_HPP

#include "ui/nodes/struct.hpp"
#include <imgui.h>

namespace Moon::ui {

struct DemoNode : public Node {
    DemoNode(const char* name, ColorTheme color = ColorTheme::Blue);
};

}  // namespace Moon::ui

#endif
