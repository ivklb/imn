
#include "demo.hpp"

#include "ui/nodes/struct.hpp"

using namespace Moon::ui;

Node Moon::ui::create_simple_node() {
    Node node("Node", ImColor(200, 100, 100));
    Pin pin_in("in", ed::PinKind::Input);
    Pin pin_out("out", ed::PinKind::Output);
    node.add_pin(pin_in);
    node.add_pin(pin_out);
    return node;
}
