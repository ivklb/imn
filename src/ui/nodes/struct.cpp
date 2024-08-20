
#include "struct.hpp"

#include <spdlog/spdlog.h>

#include <ranges>
#include <vector>

#include "core/thread_pool.hpp"
#include "ext/node_editor/imnodes.h"
#include "ui/imgui_helper.hpp"

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

Pin::Pin(Node* node, const char* name, PinKind kind, ColorTheme color)
    : node(node),
      name(name),
      kind(kind),
      color(color) {
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

Node::Node()
    : status(NodeStatus::Created),
      progress_cur(0),
      progress_max(0) {
    id = IDGenerator::next();
    body_id = IDGenerator::next();

    auto name_width = ImGui::CalcTextSize(name().c_str()).x * 1.2f;
    auto default_width = ui::get_style().font_size * 6.0f;
    width = std::max(name_width, default_width);
}

void Node::fit_json(json j_node) {
    // fit id
    id = j_node["id"].get<int>();
    body_id = j_node["body_id"].get<int>();
    for (auto& [name, pin] : inputs) {
        pin->id = j_node["inputs"][name]["id"].get<int>();
    }
    for (auto& [name, pin] : outputs) {
        pin->id = j_node["outputs"][name]["id"].get<int>();
    }

    // fit position
    auto pos_x = j_node["pos_x"].get<float>();
    auto pos_y = j_node["pos_y"].get<float>();
    ImNodes::SetNodeScreenSpacePos(id, ImVec2(pos_x, pos_y));
}

json Node::to_json() {
    json rv;
    rv["name"] = name();

    rv["id"] = id;
    rv["body_id"] = body_id;
    for (auto& [name, pin] : inputs) {
        rv["inputs"][name]["id"] = pin->id;
    }
    for (auto& [name, pin] : outputs) {
        rv["outputs"][name]["id"] = pin->id;
    }

    auto pos = ImNodes::GetNodeScreenSpacePos(id);
    rv["pos_x"] = pos.x;
    rv["pos_y"] = pos.y;
    return rv;
}

void Node::draw_frame() {
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, get_highlight_color(color()));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, get_highlight_color(color()));
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, get_normal_color(color()));

    ImNodes::BeginNode(id);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(name().c_str());
    ImNodes::EndNodeTitleBar();
    if (ImGui::BeginItemTooltip()) {
        _draw_titlebar_tooltip();
        ImGui::EndTooltip();
    }

    _draw_pins();
    _draw_process_bar();

    ImNodes::BeginStaticAttribute(body_id);
    _draw_body();
    ImNodes::EndStaticAttribute();

    ImNodes::EndNode();

    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

void Node::on_activated() {
    ImNodes::EditorContextCenterNode(id);
}

std::any Node::get_input(int pid) {
    if (!graph) {
        SPDLOG_DEBUG("Node {} has no graph", name());
        return {};
    }
    auto node = graph->get_upstream_node(pid);
    if (!node) {
        SPDLOG_DEBUG("Node {} has no upstream node", name());
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
            SPDLOG_ERROR("Node {} process error: {}", name(), e.what());
            status = NodeStatus::Error;
        }
    });
}

int Node::max_id() {
    int max_ = std::max(id, body_id);
    for (auto& [name, pin] : inputs) {
        max_ = std::max(max_, pin->id);
    }
    for (auto& [name, pin] : outputs) {
        max_ = std::max(max_, pin->id);
    }
    return max_;
}

void Node::_draw_titlebar_tooltip() {
    ImGui::Text("name: %s", name().c_str());
    ImGui::Text("id: %d", id);
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
        auto font_size = ui::get_style().font_size;
        if (progress_max == 0) {
            ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(width, font_size * 0.2f), "");
        } else {
            float fraction = float(progress_cur) / progress_max;
            ImGui::ProgressBar(fraction, ImVec2(width, font_size * 0.2f), "");
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

Graph Graph::from_json(json data) {
    Graph graph;
    int max_id = 0;
    for (auto& node_json : data["nodes"]) {
        auto name = node_json["name"].get<std::string>();
        auto node = core::ObjectFactory<Node>::create(name);
        node->fit_json(node_json);
        graph.insert_node(node);
        max_id = std::max(max_id, node->max_id());
    }
    for (auto& edge_json : data["edges"]) {
        // { id: 0, start_nid: 1, start_pid: 2, to_nid: 3, to_pid: 4 }
        auto link = graph.insert_link(
            edge_json["start_pid"].get<int>(),
            edge_json["end_pid"].get<int>());
        link->id = edge_json["id"].get<int>();
        max_id = std::max(max_id, link->id);
    }
    IDGenerator::set_next(max_id + 1);
    return graph;
}

json Graph::to_json() {
    json rv;
    for (auto& [id, node] : nodes) {
        json j_node = node->to_json();
        rv["nodes"].push_back(j_node);
    }
    for (auto& [id, link] : links) {
        json j_link;
        j_link["id"] = link->id;
        j_link["start_nid"] = link->start_nid;
        j_link["start_pid"] = link->start_pid;
        j_link["end_nid"] = link->end_nid;
        j_link["end_pid"] = link->end_pid;
        rv["edges"].push_back(j_link);
    }
    return rv;
}

// TODO: find a better way to do this
std::shared_ptr<Pin> Graph::get_pin(int pid, PinKind kind) const {
    for (auto& [id, node] : nodes) {
        if (kind == PinKind::In) {
            for (auto& [name, pin] : node->inputs) {
                if (pin->id == pid) {
                    return pin;
                }
            }
        } else if (kind == PinKind::Out) {
            for (auto& [name, pin] : node->outputs) {
                if (pin->id == pid) {
                    return pin;
                }
            }
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

std::shared_ptr<Link> Graph::insert_link(int start_pid, int end_pid) {
    if (end_pin_to_link.contains(end_pid)) {
        // TODO: break existing link?
        return nullptr;
    }

    auto from_pin = get_pin(start_pid, PinKind::Out);
    auto to_pin = get_pin(end_pid, PinKind::In);

    auto link = std::make_shared<Link>(from_pin->node->id, start_pid, to_pin->node->id, end_pid);
    links[link->id] = link;
    end_pin_to_link[to_pin->id] = link;
    if (start_pin_to_links.count(start_pid) == 0) {
        start_pin_to_links[start_pid] = std::vector<std::shared_ptr<Link>>();
    }
    start_pin_to_links[start_pid].push_back(link);
    return link;
}

void Graph::erase_link(int link_id) {
    auto link = links.at(link_id);
    end_pin_to_link.erase(link->end_pid);
    std::erase_if(start_pin_to_links[link->start_pid], [=](const auto& l) { return l->id == link_id; });
    links.erase(link_id);
}

void Graph::process() {
    for (auto& [nid, node] : nodes) {
        if (node->status == NodeStatus::Dirty) {
            for (auto& [name, pin] : node->outputs) {
                auto ns = get_downstream_nodes(pin->id);
                for (auto& n : ns) {
                    n->status = NodeStatus::Pending;
                }
            }
            node->status = NodeStatus::Done;
        }
        if (node->status == NodeStatus::Pending) {
            bool inputs_ready = true;
            for (auto& [name, pin] : node->inputs) {
                auto n = get_upstream_node(pin->id);
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

ImColor imn::ui::get_normal_color(ColorTheme color) {
    return std::get<1>(color_theme_map.at(color));
}

ImColor imn::ui::get_highlight_color(ColorTheme color) {
    return std::get<0>(color_theme_map.at(color));
}
