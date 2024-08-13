
#include "struct.hpp"

#include <spdlog/spdlog.h>

#include <vector>

#include "core/thread_pool.hpp"
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
      status(NodeStatus::Created),
      process_cur(0),
      process_max(0) {
    id = IDGenerator::next();

    auto name_width = ImGui::CalcTextSize(name).x * 1.2f;
    auto default_width = ui::get_style().font_size * 6.0f;
    width = std::max(name_width, default_width);
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

std::any Node::get_input(int pid) {
    if (!graph) {
        SPDLOG_DEBUG("Node {} has no graph", name);
        return {};
    }
    auto node = graph->get_upstream_node(pid);
    if (!node) {
        SPDLOG_DEBUG("Node {} has no upstream node", name);
        return {};
    }
    return node->get_output(pid);
}

std::any Node::get_output(int pid) {
    return {};
}

void Node::process() {
    status = NodeStatus::Processing;
    pool::enqueue([this]() {
        try {
            _process();
            status = NodeStatus::Done;
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Node {} process error: {}", name, e.what());
            status = NodeStatus::Error;
        }
    });
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

Link::Link(int start_nid, int start_pid, int end_nid, int end_pid)
    : start_nid(start_nid),
      start_pid(start_pid),
      end_nid(end_nid),
      end_pid(end_pid) {
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

std::shared_ptr<Node> Graph::get_upstream_node(int pin_id) const {
    if (end_pin_to_link.count(pin_id)) {
        auto link = end_pin_to_link.at(pin_id);
        return nodes.at(link->start_nid);
    }
    return nullptr;
}

std::vector<std::shared_ptr<Node>> Graph::get_downstream_nodes(int pin_id) const {
    std::vector<std::shared_ptr<Node>> nodes;
    if (start_pin_to_links.count(pin_id)) {
        for (auto& link : start_pin_to_links.at(pin_id)) {
            nodes.push_back(this->nodes.at(link->end_nid));
        }
    }
    return nodes;
}

int Graph::insert_node(const std::shared_ptr<Node>& node) {
    node->graph = this;
    nodes[node->id] = node;
    return node->id;
}

void Graph::erase_node(int node_id) {
    std::vector<int> links_to_remove;
    for (auto& [id, link] : links) {
        if (link->start_nid == node_id || link->end_nid == node_id) {
            links_to_remove.push_back(id);
        }
    }
    for (auto& link_id : links_to_remove) {
        erase_link(link_id);
    }
    nodes.erase(node_id);
}

int Graph::insert_link(int start_pid, int end_pid) {
    auto from_pin = pin(start_pid);
    auto to_pin = pin(end_pid);
    if (to_pin->connect_count > 0) {
        // TODO: break existing link?
        return -1;
    }

    from_pin->connect_count++;
    to_pin->connect_count++;
    auto link = std::make_shared<Link>(from_pin->node->id, start_pid, to_pin->node->id, end_pid);
    links[link->id] = link;
    end_pin_to_link[to_pin->id] = link;
    if (start_pin_to_links.count(start_pid) == 0) {
        start_pin_to_links[start_pid] = std::vector<std::shared_ptr<Link>>();
    }
    start_pin_to_links[start_pid].push_back(link);
    return link->id;
}

void Graph::erase_link(int link_id) {
    auto link = links.at(link_id);
    nodes[link->start_nid]->outputs[link->start_pid]->connect_count--;
    nodes[link->end_nid]->inputs[link->end_pid]->connect_count--;
    links.erase(link_id);
    end_pin_to_link.erase(link->end_pid);
    std::erase_if(start_pin_to_links[link->start_pid], [=](const auto& l) { return l->id == link_id; });
}

void Graph::process() {
    for (auto& [nid, node] : nodes) {
        if (node->status == NodeStatus::Dirty) {
            for (auto& [pid, pin] : node->outputs) {
                auto ns = get_downstream_nodes(pid);
                for (auto& n : ns) {
                    n->status = NodeStatus::Pending;
                }
            }
            node->status = NodeStatus::Done;
        }
        if (node->status == NodeStatus::Pending) {
            bool inputs_ready = true;
            for (auto& [pid, pin] : node->inputs) {
                auto n = get_upstream_node(pid);
                if (n == nullptr || n->status != NodeStatus::Done) {
                    inputs_ready = false;
                    break;
                }
            }
            if (inputs_ready) {
                node->process();
            }
        }
    }
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