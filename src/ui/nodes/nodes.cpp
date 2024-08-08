

#include "nodes.hpp"

#include <spdlog/spdlog.h>
#include <utf8.h>
#include <utf8/cpp20.h>

#include <opencv2/opencv.hpp>

#include "core/io.hpp"
#include "pins.hpp"
#include "ui/dialog/import_dialog.hpp"
#include "util/fs.hpp"

using namespace imn::ui;

DemoNode::DemoNode(const char* name, ColorTheme color) : Node(name, color) {
    auto pa = std::make_shared<IntPin>("a", PinKind::In, ColorTheme::Green);
    auto pb = std::make_shared<IntPin>("b", PinKind::In, ColorTheme::Red);
    auto pc = std::make_shared<IntPin>("c", PinKind::Out, ColorTheme::Blue);

    this->color = ColorTheme::Blue;

    inputs[pa->id] = pa;
    inputs[pb->id] = pb;
    outputs[pc->id] = pc;

    _build_pins();
    status = NodeStatus::Processing;
}

ImageLoaderNode::ImageLoaderNode() : Node("Image Loader", ColorTheme::Red), config({}) {
    auto p = std::make_shared<ImagePin>("image", PinKind::Out);
    outputs[p->id] = p;
    _build_pins();
}

void ImageLoaderNode::_draw_body() {
    auto button_width = (width - ImGui::GetStyle().ItemSpacing.x) * 0.3;
    auto text_width = (width - ImGui::GetStyle().ItemSpacing.x) * 0.7;

    ImGui::SetNextItemWidth(button_width);
    if (ImGui::Button("...")) {
        imn::fs::openFileBrowser(
            imn::fs::DialogMode::Open,
            {{"Image Files", "png,jpg,jpeg,bmp,tiff,tif,gif"}},
            [this](const char* p) {
                file_str = p;
                file_path = utf8::utf8to16(file_str);
            });
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(text_width);
    ImGui::InputText("##file_path", (char*)file_str.c_str(), 256, ImGuiInputTextFlags_ReadOnly);
    if (!file_str.empty() && ImGui::BeginItemTooltip()) {
        ImGui::TextUnformatted(file_str.c_str());
        ImGui::EndTooltip();
    }

    if (!file_str.empty() && !imn::io::is_image(file_path)) {
        status = NodeStatus::WaitingUserInput;
        const char* combo_items[] = {"uint8", "uint16", "uint32", "float32"};
        const int combo_values[] = {CV_8UC1, CV_16UC1, CV_32SC1, CV_32FC1};
        static int item_current = 0;
        ImGui::PushItemWidth(width);
        ImGui::Combo("image type", &item_current, combo_items, IM_ARRAYSIZE(combo_items));
        ImGui::InputInt("offset", &config.offset);
        ImGui::InputInt("width", &config.width);
        ImGui::InputInt("height", &config.height);
        ImGui::PopItemWidth();
        config.image_type = combo_values[item_current];
    }

    if (ImGui::Button("load")) {
        status = NodeStatus::Processing;
        image = imn::io::load_image(file_path, config);
        status = NodeStatus::Done;
    }
}

ImagePreviewNode::ImagePreviewNode()
    : Node("Image Preview", ColorTheme::Orange) {
    auto p = std::make_shared<ImagePin>("image", PinKind::In);
    inputs[p->id] = p;
    _build_pins();

    status = NodeStatus::WaitingLink;
}

void ImagePreviewNode::_draw_body() {
}