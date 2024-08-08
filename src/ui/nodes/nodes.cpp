

#include "nodes.hpp"

#include "pins.hpp"
#include "util/fs.hpp"

using namespace Moon::ui;

DemoNode::DemoNode(const char* name, ColorTheme color) : Node(name, color) {
    auto pa = std::make_shared<IntPin>("a", PinKind::In, ColorTheme::Green);
    auto pb = std::make_shared<IntPin>("b", PinKind::In, ColorTheme::Red);
    auto pc = std::make_shared<IntPin>("c", PinKind::Out, ColorTheme::Blue);

    this->color = ColorTheme::Blue;

    inputs[pa->id] = pa;
    inputs[pb->id] = pb;
    outputs[pc->id] = pc;

    _build_pins();
}

ImageLoaderNode::ImageLoaderNode() : Node("Image Loader", ColorTheme::Red) {
    auto p = std::make_shared<ImagePin>("image", PinKind::Out);
    outputs[p->id] = p;
    _build_pins();
}

void ImageLoaderNode::_draw_static() {
    if (ImGui::Button("...")) {
        imn::fs::openFileBrowser(
            imn::fs::DialogMode::Open,
            {
                {"Image Files", "png,jpg,jpeg,bmp,tiff,tif,gif"},
            },
            [this](const std::filesystem::path& path) {
                file_path = path;
            });
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50);
    ImGui::InputText("##file_path", (char*)file_path.string().c_str(), 256, ImGuiInputTextFlags_ReadOnly);
    if (!file_path.empty() && ImGui::BeginItemTooltip()) {
        ImGui::TextUnformatted(file_path.string().c_str());
        ImGui::EndTooltip();
    }
}
