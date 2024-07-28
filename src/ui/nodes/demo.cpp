
#include "demo.hpp"

#include "ui/nodes/struct.hpp"

using namespace Moon::ui;

Node Moon::ui::create_simple_node() {
    auto color = ImColor(230, 42, 82);
    Node node("Node", color);
    Pin pin_in("in", ed::PinKind::Input, color);
    Pin pin_out("out", ed::PinKind::Output, color);
    node.add_pin(pin_in);
    node.add_pin(pin_out);
    return node;
}
