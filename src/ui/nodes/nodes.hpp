
#ifndef UI__NODES__NODES_HPP
#define UI__NODES__NODES_HPP

#include "ui/nodes/struct.hpp"
#include <imgui.h>

namespace Moon::ui {

struct DemoNode : public Node {
    DemoNode(const char* name, ImColor color = ImColor(255, 255, 255));
};

}  // namespace Moon::ui

#endif
