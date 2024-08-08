
#include "struct.hpp"

#include <vector>

#include "ui/node_editor/imnodes.h"
#include "ui/style.hpp"

using namespace imn::ui;

// https://flatuicolors.com/palette/defo
std::map<ColorTheme, std::tuple<ImColor, ImColor>> color_theme_map = {
    // {ColorTheme::Cyan, {ImColor(0x1abc9c), ImColor(0x16a085)}},
    // {ColorTheme::Green, {ImColor(0x2ecc71), ImColor(0x27ae60)}},
    // {ColorTheme::Blue, {ImColor(0x3498db), ImColor(0x2980b9)}},
    // {ColorTheme::Purple, {ImColor(0x9b59b6), ImColor(0x8e44ad)}},
    // {ColorTheme::Midnight, {ImColor(0x34495e), ImColor(0x2c3e50)}},
    // {ColorTheme::Yellow, {ImColor(0xf1c40f), ImColor(0xf39c12)}},
    // {ColorTheme::Orange, {ImColor(0xe67e22), ImColor(0xd35400)}},
    // {ColorTheme::Red, {ImColor(0xe74c3c), ImColor(0xc0392b)}},
    // {ColorTheme::White, {ImColor(0xecf0f1), ImColor(0xbdc3c7)}},
    // {ColorTheme::Gray, {ImColor(0x95a5a6), ImColor(0x7f8c8d)}},
    {ColorTheme::Cyan, {ImColor(26, 188, 156), ImColor(22, 160, 133)}},
    {ColorTheme::Green, {ImColor(46, 204, 113), ImColor(39, 174, 96)}},
    {ColorTheme::Blue, {ImColor(52, 152, 219), ImColor(41, 128, 185)}},
    {ColorTheme::Purple, {ImColor(155, 89, 182), ImColor(142, 68, 173)}},
    {ColorTheme::Midnight, {ImColor(52, 73, 94), ImColor(44, 62, 80)}},
    {ColorTheme::Yellow, {ImColor(241, 196, 15), ImColor(243, 156, 18)}},
    {ColorTheme::Orange, {ImColor(230, 126, 34), ImColor(211, 84, 0)}},
    {ColorTheme::Red, {ImColor(231, 76, 60), ImColor(192, 57, 43)}},
    {ColorTheme::White, {ImColor(236, 240, 241), ImColor(189, 195, 199)}},
    {ColorTheme::Gray, {ImColor(149, 165, 166), ImColor(127, 140, 141)}},
};

Pin::Pin(const char* name, PinKind kind, ColorTheme color)
    : node(nullptr),
      name(name),
      kind(kind),
      color(color),
      connect_count(0) {
    id = IDGenerator::next();
}

void Pin::draw_frame() {
    const float node_width = node->width;
    const float label_width = ImGui::CalcTextSize(name.c_str()).x;

    ImNodes::PushColorStyle(ImNodesCol_PinHovered, get_highlight_color(color));
    ImNodes::PushColorStyle(ImNodesCol_Pin, get_normal_color(color));
    if (kind == PinKind::In) {
        ImNodes::BeginInputAttribute(id);
        ImGui::TextUnformatted(name.c_str());
        ImNodes::EndInputAttribute();
    } else if (kind == PinKind::Out) {
        ImNodes::BeginOutputAttribute(id);
        ImGui::Indent(node_width - label_width);
        ImGui::TextUnformatted(name.c_str());
        ImNodes::EndOutputAttribute();
    }
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

Node::Node(const char* name, ColorTheme color)
    : name(name),
      color(color),
      status(NodeStatus::Idle),
      width(100),
      process_cur(0),
      process_max(0) {
    id = IDGenerator::next();
}

void Node::draw_frame() {
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, get_highlight_color(color));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, get_highlight_color(color));
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, get_normal_color(color));

    ImNodes::BeginNode(id);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(name.c_str());
    ImNodes::EndNodeTitleBar();
    if (ImGui::BeginItemTooltip()) {
        _draw_titlebar_tooltip();
        ImGui::EndTooltip();
    }

    _draw_pins();
    _draw_process_bar();
    _draw_body();

    ImNodes::EndNode();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

void Node::_build_pins() {
    for (auto& [id, pin] : inputs) {
        pin->node = this;
    }
    for (auto& [id, pin] : outputs) {
        pin->node = this;
    }
}

void Node::_draw_titlebar_tooltip() {
    ImGui::TextUnformatted(name.c_str());
}

void Node::_draw_pins() {
    for (auto& [id, pin] : inputs) {
        pin->draw_frame();
    }
    for (auto& [id, pin] : outputs) {
        pin->draw_frame();
    }
}

void Node::_draw_process_bar() {
    if (status == NodeStatus::Processing) {
        if (process_max == 0) {
            ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(width, 5.f), "");
        } else {
            float fraction = float(process_cur) / process_max;
            ImGui::ProgressBar(process_cur, ImVec2(-1, 0), "");
        }
    }
}

Link::Link(int from_nid, int from_pid, int to_nid, int to_pid)
    : from_nid(from_nid),
      from_pid(from_pid),
      to_nid(to_nid),
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

std::vector<std::shared_ptr<Link>> Graph::input_links_from_node(int node_id) const {
    // TODO: fix me
    return {};
}

std::vector<std::shared_ptr<Link>> Graph::output_links_from_node(int node_id) const {
    // TODO: fix me
    return {};
}

int Graph::insert_node(const std::shared_ptr<Node>& node) {
    nodes[node->id] = node;
    return node->id;
}

void Graph::erase_node(int node_id) {
    std::vector<int> links_to_remove;
    for (auto& [id, link] : links) {
        if (link->from_nid == node_id || link->to_nid == node_id) {
            links_to_remove.push_back(id);
        }
    }
    for (auto& link_id : links_to_remove) {
        erase_link(link_id);
    }
    nodes.erase(node_id);
}

int Graph::insert_link(int from_pid, int to_pid) {
    auto from_pin = pin(from_pid);
    auto to_pin = pin(to_pid);
    if (to_pin->connect_count > 0) {
        // TODO: break existing link?
        return -1;
    }

    from_pin->connect_count++;
    to_pin->connect_count++;
    auto link = std::make_shared<Link>(from_pin->node->id, from_pid, to_pin->node->id, to_pid);
    links[link->id] = link;
    return link->id;
}

void Graph::erase_link(int link_id) {
    auto link = links.at(link_id);
    nodes[link->from_nid]->outputs[link->from_pid]->connect_count--;
    nodes[link->to_nid]->inputs[link->to_pid]->connect_count--;
    links.erase(link_id);
}

int IDGenerator::_next_id = 0;
int IDGenerator::next() {
    return _next_id++;
}

void IDGenerator::set_next(int id) {
    _next_id = id;
}

ImColor imn::ui::get_normal_color(ColorTheme color) {
    return std::get<1>(color_theme_map.at(color));
}

ImColor imn::ui::get_highlight_color(ColorTheme color) {
    return std::get<0>(color_theme_map.at(color));
}