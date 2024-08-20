

#ifndef UI__NODES__STRUCT_HPP
#define UI__NODES__STRUCT_HPP

#include <imgui.h>

#include <any>
#include <atomic>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <vector>

#include "core/object.hpp"

using json = nlohmann::json;
namespace imn::ui {

// https://hackernoon.com/zh/%E5%A6%82%E4%BD%95%E9%80%89%E6%8B%A9%E9%A2%9C%E8%89%B2%E5%B9%B6%E7%A1%AE%E5%AE%9A%E4%B8%8D%E5%90%8C%E7%8A%B6%E6%80%81%E7%9A%84%E8%89%B2%E8%B0%83%E6%82%AC%E5%81%9C%E6%B4%BB%E5%8A%A8%E6%8C%89%E4%B8%8B%E7%A6%81%E7%94%A8
enum class ColorTheme {
    Cyan,
    Green,
    Blue,
    Purple,
    Midnight,
    Yellow,
    Orange,
    Red,
    White,
    Gray,
};

enum class PinKind {
    In,
    Out,
};

enum class NodeStatus {
    Created,
    Dirty,
    Processing,
    Pending,
    WaitingUserInput,
    WaitingNodeInput,
    Done,
    Error,
};

struct Node;
struct Graph;

struct Pin {
    Node* node;
    int id;
    std::string name;
    PinKind kind;
    ColorTheme color;

    Pin(Node* node, const char* name, PinKind kind, ColorTheme color = ColorTheme::Blue);
    virtual ~Pin() {}
    virtual void draw_frame();
};

struct Node {
    Graph* graph;
    int id;
    int body_id;
    std::map<std::string, std::shared_ptr<Pin>> inputs;  // name -> pin
    std::map<std::string, std::shared_ptr<Pin>> outputs;  // name -> pin
    NodeStatus status;
    std::atomic_int progress_cur;
    std::atomic_int progress_max;

    Node();
    virtual ~Node() {}
    virtual void fit_json(json data);
    virtual json to_json();
    virtual std::string name() { return "Node"; }
    virtual ColorTheme color() { return ColorTheme::Blue; }
    // TODO: rename to node_width()
    virtual float width();
    virtual void draw_frame();
    virtual void on_activated();
    virtual void on_click() {}
    virtual void on_double_click() {}
    virtual std::any get_input(int pid);
    template <typename T>
    T get_input(int pid);
    virtual std::any get_output(int pid);
    virtual void process();
    int max_id();

   protected:
    virtual void _process() {}
    virtual void _draw_titlebar_tooltip();
    virtual void _draw_pins();
    virtual void _draw_process_bar();
    virtual void _draw_body() {}
};

template <typename T>
T Node::get_input(int pid) {
    return std::any_cast<T>(get_input(pid));
}

struct Link {
    int id;
    int start_nid;
    int start_pid;
    int end_nid;
    int end_pid;

    // Link() {}  // used in std::map
    Link(int start_nid, int start_pid, int end_nid, int end_pid);
};

struct Graph {
    std::map<int, std::shared_ptr<Node>> nodes;
    std::map<int, std::shared_ptr<Link>> links;
    std::map<int, std::shared_ptr<Link>> end_pin_to_link;
    std::map<int, std::vector<std::shared_ptr<Link>>> start_pin_to_links;

    // Ctors
    Graph() {}
    static Graph from_json(json data);
    json to_json();

    // Element Access
    std::shared_ptr<Pin> get_pin(int pid, PinKind kind) const;

    // Capacity
    std::shared_ptr<Node> get_upstream_node(int pin_id) const;
    std::vector<std::shared_ptr<Node>> get_downstream_nodes(int pin_id) const;

    // Modifiers
    int insert_node(const std::shared_ptr<Node>& node);
    void erase_node(int node_id);
    std::shared_ptr<Link> insert_link(int start_pid, int end_pid);
    void erase_link(int link_id);

    void process();
};

ImColor get_normal_color(ColorTheme color);
ImColor get_highlight_color(ColorTheme color);

}  // namespace imn::ui
#endif
