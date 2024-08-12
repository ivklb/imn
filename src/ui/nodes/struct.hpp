

#ifndef UI__NODES__STRUCT_HPP
#define UI__NODES__STRUCT_HPP

#include <imgui.h>

#include <any>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

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
    int connect_count;

    Pin(const char* name, PinKind kind, ColorTheme color = ColorTheme::Blue);
    virtual ~Pin() {}
    virtual void draw_frame();
};

struct Node {
    Graph* graph;
    int id;
    std::string name;
    std::map<int, std::shared_ptr<Pin>> inputs;
    std::map<int, std::shared_ptr<Pin>> outputs;
    ColorTheme color;
    NodeStatus status;
    float width;
    int process_cur;
    int process_max;

    Node(const char* name, ColorTheme color = ColorTheme::Blue);
    virtual ~Node() {}
    virtual void draw_frame();
    virtual std::any get_input(int pid);
    template <typename T>
    T get_input(int pid);
    virtual std::any get_output(int pid);
    virtual void process();

   protected:
    virtual void _process() {}
    virtual void _build_pins();
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

    // Element Access
    std::shared_ptr<Pin> pin(int pid) const;

    // Capacity
    std::shared_ptr<Node> get_upstream_node(int pin_id) const;
    std::vector<std::shared_ptr<Node>> get_downstream_nodes(int pin_id) const;

    // Modifiers
    int insert_node(const std::shared_ptr<Node>& node);
    void erase_node(int node_id);
    int insert_link(int start_pid, int end_pid);
    void erase_link(int link_id);

    void process();
};

struct IDGenerator {
    static int next();
    static void set_next(int id);

   private:
    static int _next_id;
};

ImColor get_normal_color(ColorTheme color);
ImColor get_highlight_color(ColorTheme color);

}  // namespace imn::ui
#endif
