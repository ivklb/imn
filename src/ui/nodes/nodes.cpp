

#include "nodes.hpp"

#include <spdlog/spdlog.h>
#include <utf8.h>
#include <utf8/cpp20.h>

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

ImageLoaderNode::ImageLoaderNode() : Node("Image Loader", ColorTheme::Red) {
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
                std::u16string pp = utf8::utf8to16(std::string(p));
                std::filesystem::path path(pp);

                if (imn::io::is_image(path)) {
                    image = imn::io::load_image(path, {});
                    file_path = p;
                } else {
                    auto [ok, conf] = show_import_dialog({});
                    if (ok) {
                        image = imn::io::load_image(path, conf);
                        file_path = p;
                    }
                }
            });
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(text_width);
    ImGui::InputText("##file_path", (char*)file_path.c_str(), 256, ImGuiInputTextFlags_ReadOnly);
    if (!file_path.empty() && ImGui::BeginItemTooltip()) {
        ImGui::TextUnformatted(file_path.c_str());
        ImGui::EndTooltip();
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