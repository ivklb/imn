
#include "struct.hpp"

#include <vector>

#include "ui/node_editor/imnodes.h"
#include "ui/style.hpp"

using namespace Moon::ui;

Pin::Pin(const char* name, PinKind kind, ImColor color)
    : node(nullptr),
      name(name),
      kind(kind),
      color(color),
      connected(false) {
    id = IDGenerator::next();
}

void Pin::draw_frame() {
    int alpha = 255;
    float icon_size = ui::get_style().font_size;

    if (kind == PinKind::In) {
        ImNodes::BeginInputAttribute(id);
        ImGui::TextUnformatted(name.c_str());
        ImNodes::EndInputAttribute();
    } else if (kind == PinKind::Out) {
        ImNodes::BeginOutputAttribute(id);
        ImGui::TextUnformatted(name.c_str());
        ImNodes::EndOutputAttribute();
    }
}

Node::Node(const char* name, ImColor color)
    : name(name),
      color(color) {
    id = IDGenerator::next();
}

void Node::draw_frame() {
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, color);
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, color);

    ImNodes::BeginNode(id);

    ImGui::BeginGroup();
    ImGui::Text(name.c_str());
    ImGui::EndGroup();

    _draw_pins();
    _draw_static();

    ImNodes::EndNode();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("State: %s", "123");
    }
}

void Node::_build_pins() {
    for (auto& [id, pin] : inputs) {
        pin->node = this;
    }
    for (auto& [id, pin] : outputs) {
        pin->node = this;
    }
}

void Node::_draw_pins() {
    for (auto& [id, pin] : inputs) {
        pin->draw_frame();
    }
    for (auto& [id, pin] : outputs) {
        pin->draw_frame();
    }
}

void Node::_draw_static() {
}

Link::Link(int from_pid, int to_pid)
    : from_pid(from_pid),
      to_pid(to_pid) {
    id = IDGenerator::next();
}

std::shared_ptr<Pin> Graph::pin(int pid) const {
    for (auto& [id, node] : nodes) {
        if (node->inputs.count(pid)) {
            return node->inputs.at(pid);
        }
        if (node->outputs.count(pid)) {
            return node->outputs.at(pid);
        }
    }
    return nullptr;
}

size_t Graph::num_edges_from_node(int node_id) const {
    // TODO: fix me
    return 0;
}

int Graph::insert_node(const std::shared_ptr<Node>& node) {
    nodes[node->id] = node;
    return node->id;
}

void Graph::erase_node(int node_id) {
    nodes.erase(node_id);
    // TODO: remove edges
}

int Graph::insert_edge(int from_pid, int to_pid) {
    auto link = std::make_shared<Link>(from_pid, to_pid);
    links[link->id] = link;
    return link->id;
}

void Graph::erase_edge(int edge_id) {
    links.erase(edge_id);
}

int IDGenerator::_next_id = 0;
int IDGenerator::next() {
    return _next_id++;
}
void IDGenerator::set_next(int id) {
    _next_id = id;
}