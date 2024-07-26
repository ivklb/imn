


#include "node.hpp"
#include "ui/node_editor/imgui_node_editor/imgui_node_editor.h"

namespace ed = ax::NodeEditor;

using namespace ax;

using ax::Widgets::IconType;


enum class PinType {
    Flow,
    Bool,
    Int,
    Float,
    String,
    Object,
    Function,
    Delegate,
};

enum class PinKind {
    Output,
    Input
};

struct Node;

struct Pin {
    ed::PinId   id;
    ::Node* node;
    std::string name;
    PinType     type;
    PinKind     kind;

    Pin(int id, const char* name, PinType type) :
        ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input) {
    }
};

struct Node {
    ed::NodeId id;
    std::string name;
    std::vector<Pin> inputs;
    std::vector<Pin> outputs;
    ImColor color;
    ImVec2 size;

    std::string State;
    std::string SavedState;

    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
        id(id), name(name), color(color), size(0, 0) {
    }
};

struct Link {
    ed::LinkId id;
    ed::PinId start_pid;
    ed::PinId end_pid;
    ImColor color;

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId) :
        id(id), start_pid(startPinId), end_pid(endPinId), color(255, 255, 255) {
    }
};


