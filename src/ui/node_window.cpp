
#include "node_window.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include "core/setting.hpp"
#include "include/def.hpp"
#include "util/common.hpp"
#include "util/imgui_util.hpp"
#include "ui/widget/common_widgets.hpp"
#include "ui/style.hpp"
#include "ui/node_editor/imgui_node_editor/utilities/widgets.h"

using namespace Moon::ui;
namespace ed = ax::NodeEditor;

NodeWindow::NodeWindow() {
    ed::Config config;
    config.SettingsFile = "Simple.json";
    m_Context = ed::CreateEditor(&config);
}

NodeWindow::~NodeWindow() {
    ed::DestroyEditor(m_Context);
}

void NodeWindow::show() {

    ed::SetCurrentEditor(m_Context);
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
    float icon_size = 24;
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
        // HeaderMin - ImVec2(padding.x - halfBorderWidth, padding.y - halfBorderWidth),
        // ImVec2(HeaderMax2.x - halfBorderWidth, HeaderMax.y),
        h_color,
        ed::GetStyle().NodeRounding,
        ImDrawFlags_RoundCornersTop

    );

    // ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1, 1, 1, 1.0f));
    // ed::PopStyleColor();
    // ImGui::PopStyleColor();
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


    for (auto& linkInfo : m_Links) {
        ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);
        // m_Context->FindLink(linkInfo.Id);
        // ed::DeleteLink(linkInfo.Id);
        // ed::Flow(linkInfo.Id);
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

            if (inputPinId && outputPinId) // both are valid, let's accept link
            {
                // ed::AcceptNewItem() return true when user release mouse button.
                if (ed::AcceptNewItem()) {
                    // Since we accepted new link, lets add one to our list of links.
                    m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });

                    // Draw new link.
                    ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
                }

                // You may choose to reject connection between these nodes
                // by calling ed::RejectNewItem(). This will allow editor to give
                // visual feedback by changing link thickness and color.
            }
        }
    }
    ed::EndCreate(); // Wraps up object creation action handling.


    // Handle deletion action
    if (ed::BeginDelete()) {
        // There may be many links marked for deletion, let's loop over them.
        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId)) {
            // If you agree that link can be deleted, accept deletion.
            if (ed::AcceptDeletedItem()) {
                // Then remove link from your data.
                for (auto& link : m_Links) {
                    if (link.Id == deletedLinkId) {
                        // ed::DeleteLink(link.Id);
                        m_Links.erase(&link);
                        break;
                    }
                }
            }

            // You may reject link deletion by calling:
            // ed::RejectDeletedItem();
        }
    }
    ed::EndDelete(); // Wrap up deletion action

    ed::End();
    ed::SetCurrentEditor(nullptr);
}
