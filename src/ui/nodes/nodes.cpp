

#include "nodes.hpp"

#include "pins.hpp"

using namespace Moon::ui;

DemoNode::DemoNode(const char* name, ImColor color) : Node(name, color) {
    auto pa = std::make_shared<IntPin>("a", PinKind::In);
    auto pb = std::make_shared<IntPin>("b", PinKind::In);
    auto pc = std::make_shared<IntPin>("c", PinKind::Out);

    inputs[pa->id] = pa;
    inputs[pb->id] = pb;
    outputs[pc->id] = pc;

    _build_pins();
}
