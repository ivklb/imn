

#include "nodes.hpp"

#include "pins.hpp"

using namespace Moon::ui;

DemoNode::DemoNode(const char* name, ColorTheme color) : Node(name, color) {
    auto pa = std::make_shared<IntPin>("a", PinKind::In, ColorTheme::Green);
    auto pb = std::make_shared<IntPin>("b", PinKind::In, ColorTheme::Red);
    auto pc = std::make_shared<IntPin>("c", PinKind::Out, ColorTheme::Blue);

    this->color = ColorTheme::Red;

    inputs[pa->id] = pa;
    inputs[pb->id] = pb;
    outputs[pc->id] = pc;

    _build_pins();
}
