

#ifndef UI__NODE_EDITOR__NODE_HPP
#define UI__NODE_EDITOR__NODE_HPP

#include <string>
#include <vector>

#include "ui/imgui_node_editor/imgui_node_editor.h"
#include "ui/imgui_node_editor/utilities/widgets.h"

namespace ed = ax::NodeEditor;
using ax::Widgets::IconType;

namespace Moon::ui {

struct Node;

struct Pin {
    Node* node;
    ed::PinId id;
    std::string name;
    ed::PinKind kind;
    ImColor color;
    IconType type;
    bool connected;

    Pin(const char* name, ed::PinKind kind, ImColor color = ImColor(255, 255, 255), IconType type = IconType::Circle);
    void on_frame();
};

struct Node {
    ed::NodeId id;
    std::string name;
    std::vector<Pin> inputs;
    std::vector<Pin> outputs;
    ImColor color;
    ImVec2 size;

    std::string state;
    std::string saved_state;

    Node(const char* name, ImColor color = ImColor(255, 255, 255));
    void add_pin(Pin p);
    void add_pins(std::vector<Pin> ps);

    void on_frame();
};

struct Link {
    ed::LinkId id;
    ed::PinId start_pid;
    ed::PinId end_pid;
    ImColor color;

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId) : id(id), start_pid(startPinId), end_pid(endPinId), color(255, 255, 255) {
    }
};

}  // namespace Moon::ui
#endif
