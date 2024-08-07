

#ifndef UI__NODES__STRUCT_HPP
#define UI__NODES__STRUCT_HPP

#include <imgui.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Moon::ui {

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
    ImColor color;
    bool connected;

    Pin(const char* name, PinKind kind, ImColor color = ImColor(255, 255, 255));
    virtual ~Pin() {}
    virtual void draw_frame();
};

struct Node {
    int id;
    std::string name;
    std::map<int, std::shared_ptr<Pin>> inputs;
    std::map<int, std::shared_ptr<Pin>> outputs;
    ImColor color;
    NodeStatus status;

    // Node() {}  // used in std::map
    Node(const char* name, ImColor color = ImColor(255, 255, 255));
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
    size_t num_edges_from_node(int node_id) const;

    // Modifiers
    int insert_node(const std::shared_ptr<Node>& node);
    void erase_node(int node_id);
    int insert_edge(int from_pid, int to_pid);
    void erase_edge(int edge_id);
};

struct IDGenerator {
    static int next();
    static void set_next(int id);

   private:
    static int _next_id;
};

}  // namespace Moon::ui
#endif
