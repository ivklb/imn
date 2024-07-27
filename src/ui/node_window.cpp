
#include "node_window.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include "core/setting.hpp"
#include "include/def.hpp"
#include "ui/imgui_node_editor/utilities/widgets.h"
#include "ui/style.hpp"
#include "ui/widget/common_widgets.hpp"
#include "util/common.hpp"
#include "util/imgui_util.hpp"

using namespace Moon::ui;
namespace ed = ax::NodeEditor;

NodeWindow::NodeWindow() {
    ed::Config config;
    config.SettingsFile = "Simple.json";
    _context = ed::CreateEditor(&config);
}

NodeWindow::~NodeWindow() {
    ed::DestroyEditor(_context);
}

void NodeWindow::show() {
    ed::SetCurrentEditor(_context);
    ed::Begin("##MyEditor", ImVec2(0.0, 0.0f));

    int uniqueId = 1;
    int uniqueIdAAA = 1000;
    ed::BeginNode(uniqueIdAAA);

    ImGui::BeginGroup();
    ImGui::Text("Node AAAA ");

    auto padding2 = ed::GetStyle().NodePadding;
    ImGui::Dummy(ImVec2(0.0f, padding2.y / 2));
    ImGui::EndGroup();
    auto HeaderMin = ImGui::GetItemRectMin();
    auto HeaderMax = ImGui::GetItemRectMax();

    ed::BeginPin(uniqueId++, ed::PinKind::Input);
    float icon_size = ui::get_style().font_size;
    auto iconType = ax::Drawing::IconType::Circle;
    bool connected = true;
    auto color = ImColor(230, 42, 82);
    int alpha = 255;

    ax::Widgets::Icon(ImVec2(icon_size, icon_size), iconType, connected, color, ImColor(32, 32, 32, alpha));
    ImGui::SameLine();
    ImGui::Text("-> in");
    ed::EndPin();

    ImGui::SameLine();

    ed::BeginPin(uniqueId++, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    // ImGui::Button("Button");
    ed::EndNode();

    auto HeaderMin2 = ImGui::GetItemRectMin();
    auto HeaderMax2 = ImGui::GetItemRectMax();
    auto drawList = ed::GetNodeBackgroundDrawList(uniqueIdAAA);
    auto h_color = ImColor(230, 42, 82);

    auto padding = ed::GetStyle().NodePadding;
    const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth;
    // const auto halfBorderWidth = 0;

    drawList->AddRectFilled(HeaderMin, HeaderMax, h_color);
    drawList->AddRectFilled(
        HeaderMin - ImVec2(padding.x - halfBorderWidth, padding.y - halfBorderWidth),
        ImVec2(HeaderMax2.x - halfBorderWidth, HeaderMax.y),
        h_color,
        ed::GetStyle().NodeRounding,
        ImDrawFlags_RoundCornersTop);

    if (ImGui::IsItemHovered()) {
        ed::Suspend();
        ImGui::SetTooltip("State: %s", "123");
        ed::Resume();
    }

    ed::BeginNode(uniqueId++);
    ImGui::Text("Node A");
    ed::BeginPin(uniqueId++, ed::PinKind::Input);
    ax::Widgets::Icon(ImVec2(icon_size, icon_size), iconType, connected, color, ImColor(32, 32, 32, alpha));
    ed::EndPin();
    ImGui::SameLine();
    ed::BeginPin(uniqueId++, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();
    ed::EndNode();

    for (auto& linkInfo : _links) {
        ed::Link(linkInfo.id, linkInfo.start_pid, linkInfo.end_pid);
    }

    if (ed::BeginCreate()) {
        ed::PinId inputPinId, outputPinId;
        if (ed::QueryNewLink(&inputPinId, &outputPinId)) {
            // QueryNewLink returns true if editor want to create new link between pins.
            //
            // Link can be created only for two valid pins, it is up to you to
            // validate if connection make sense. Editor is happy to make any.
            //
            // Link always goes from input to output. User may choose to drag
            // link from output pin or input pin. This determine which pin ids
            // are valid and which are not:
            //   * input valid, output invalid - user started to drag new ling from input pin
            //   * input invalid, output valid - user started to drag new ling from output pin
            //   * input valid, output valid   - user dragged link over other pin, can be validated

            if (inputPinId && outputPinId)  // both are valid, let's accept link
            {
                // ed::AcceptNewItem() return true when user release mouse button.
                if (ed::AcceptNewItem()) {
                    _links.push_back({ed::LinkId(_next_link_id++), inputPinId, outputPinId});

                    // Draw new link.
                    ed::Link(_links.back().id, _links.back().start_pid, _links.back().end_pid);
                }

                // You may choose to reject connection between these nodes
                // by calling ed::RejectNewItem(). This will allow editor to give
                // visual feedback by changing link thickness and color.
            }
        }
    }
    ed::EndCreate();  // Wraps up object creation action handling.

    // Handle deletion action
    if (ed::BeginDelete()) {
        ed::NodeId nodeId = 0;
        while (ed::QueryDeletedNode(&nodeId)) {
            if (ed::AcceptDeletedItem()) {
                auto id = std::find_if(_nodes.begin(), _nodes.end(), [nodeId](auto& node) { return node.id == nodeId; });
                if (id != _nodes.end()) {
                    _nodes.erase(id);
                    break;
                }
            }
        }

        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId)) {
            if (ed::AcceptDeletedItem()) {
                auto id = std::find_if(_links.begin(), _links.end(), [=](auto& link) { return link.id == deletedLinkId; });
                if (id != _links.end()) {
                    _links.erase(id);
                    break;
                }
            }
        }
    }
    ed::EndDelete();  // Wrap up deletion action

    ed::End();
    ed::SetCurrentEditor(nullptr);
}
