
#ifndef UI__NODE_WINDOW_HPP
#define UI__NODE_WINDOW_HPP

#include <map>
#include <memory>
#include <tuple>
#include <vector>

#include <imgui.h>
#include <implot.h>
#include <opencv2/opencv.hpp>
#include "base_window.hpp"
#include "ui/node_editor/imgui_node_editor/imgui_node_editor.h"

namespace ed = ax::NodeEditor;

namespace Moon::ui {

struct LinkInfo {
    ed::LinkId Id;
    ed::PinId  InputId;
    ed::PinId  OutputId;
};

class NodeWindow : public BaseWindow {
public:
    NodeWindow();
    ~NodeWindow();
    void show() override;

private:
    ImVector<LinkInfo>   m_Links;
    int                  m_NextLinkId = 100;
    ed::EditorContext* m_Context = nullptr;
};

}


#endif

