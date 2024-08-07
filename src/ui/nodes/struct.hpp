

#ifndef UI__NODES__STRUCT_HPP
#define UI__NODES__STRUCT_HPP

#include <imgui.h>

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace Moon::ui {

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
    Idle,
    Pending,
    Processing,
    WaitingUserInput,
    Done,
    Error,
};

struct Node;

struct Pin {
    Node* node;
    int id;
    std::string name;
    PinKind kind;
    ColorTheme color;
    bool connected;

    Pin(const char* name, PinKind kind, ColorTheme color = ColorTheme::Blue);
    virtual ~Pin() {}
    virtual void draw_frame();
};

struct Node {
    int id;
    std::string name;
    std::map<int, std::shared_ptr<Pin>> inputs;
    std::map<int, std::shared_ptr<Pin>> outputs;
    ColorTheme color;
    NodeStatus status;

    Node(const char* name, ColorTheme color = ColorTheme::Blue);
    virtual ~Node() {}
    virtual void draw_frame();

   protected:
    void _build_pins();

   private:
    virtual void _draw_pins();
    virtual void _draw_static();
};

struct Link {
    int id;
    int from_pid;
    int to_pid;

    // Link() {}  // used in std::map
    Link(int from_pid, int to_pid);
};

struct Graph {
    std::map<int, std::shared_ptr<Node>> nodes;
    std::map<int, std::shared_ptr<Link>> links;

    // Element Access
    std::shared_ptr<Pin> pin(int pid) const;

    // Capacity
    std::vector<std::shared_ptr<Link>> input_links_from_node(int node_id) const;
    std::vector<std::shared_ptr<Link>> output_links_from_node(int node_id) const;

    // Modifiers
    int insert_node(const std::shared_ptr<Node>& node);
    void erase_node(int node_id);
    int insert_link(int from_pid, int to_pid);
    void erase_link(int link_id);
};

struct IDGenerator {
    static int next();
    static void set_next(int id);

   private:
    static int _next_id;
};

ImColor get_normal_color(ColorTheme color);
ImColor get_highlight_color(ColorTheme color);


}  // namespace Moon::ui
#endif
