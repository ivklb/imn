

#include "nodes.hpp"

#include <spdlog/spdlog.h>
#include <utf8.h>
#include <utf8/cpp20.h>

#include <cassert>
#include <opencv2/opencv.hpp>

#include "core/io.hpp"
#include "core/lambda.hpp"
#include "core/thread_pool.hpp"
#include "ext/imgui_notify/ImGuiNotify.hpp"
#include "pins.hpp"
#include "ui/imgui_helper.hpp"
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

std::any ImageLoaderNode::get_output(int pid) {
    assert(status == NodeStatus::Done && "ImageLoaderNode::get_output() called before processing is done");
    return image;
}

void ImageLoaderNode::_draw_body() {
    auto button_width = (width - ImGui::GetStyle().ItemSpacing.x) * 0.3f;
    auto text_width = (width - ImGui::GetStyle().ItemSpacing.x) * 0.7f;

    if (ImGui::Button("...", ImVec2(button_width, 0))) {
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
        const char* combo_items[] = {"uint8", "uint16", "uint32", "float32"};
        const int combo_values[] = {CV_8UC1, CV_16UC1, CV_32SC1, CV_32FC1};
        static int item_current = 0;
        ImGui::PushItemWidth(width - ImGui::CalcTextSize("height").x - ImGui::GetStyle().ItemSpacing.x);
        ImGui::Combo("type", &item_current, combo_items, IM_ARRAYSIZE(combo_items));
        ImGui::InputInt("offset", &config.offset);
        ImGui::InputInt("width", &config.width);
        ImGui::InputInt("height", &config.height);
        ImGui::PopItemWidth();
        config.image_type = combo_values[item_current];
    }

    if (ImGui::Button("load")) {
        if (file_str.empty()) {
            ImGui::InsertNotification({ImGuiToastType::Warning, 3000, "! %s", "Please select a file"});
        } else {
            status = NodeStatus::Processing;
            image = imn::io::load_image(file_path, config);
            status = NodeStatus::Dirty;
        }
    }
}

VolumeLoaderNode::VolumeLoaderNode() : Node("Volume Loader", ColorTheme::Red), config({}) {
    auto p = std::make_shared<VolumePin>("volume", PinKind::Out);
    outputs[p->id] = p;
    _build_pins();

    width = ui::get_style().font_size * 8.0f;
    _item_current = 0;
}

std::any VolumeLoaderNode::get_output(int pid) {
    assert(status == NodeStatus::Done && "ImageLoaderNode::get_output() called before processing is done");
    return volume;
}

void VolumeLoaderNode::_draw_body() {
    auto button_width = (width - ImGui::GetStyle().ItemSpacing.x) * 0.3f;
    auto text_width = (width - ImGui::GetStyle().ItemSpacing.x) * 0.7f;

    if (ImGui::Button("...", ImVec2(button_width, 0))) {
        imn::fs::openFileBrowser(imn::fs::DialogMode::Open, {}, [this](const char* p) {
            file_str = p;
            file_path = utf8::utf8to16(file_str);
            config = io::parse_filename(file_str);
            if (config.image_type == CV_8UC1) {
                _item_current = 0;
            } else if (config.image_type == CV_16UC1) {
                _item_current = 1;
            } else if (config.image_type == CV_32SC1) {
                _item_current = 2;
            } else if (config.image_type == CV_32FC1) {
                _item_current = 3;
            } else {
                _item_current = 0;
            }
        });
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(text_width);
    ImGui::InputText("##file_path", (char*)file_str.c_str(), 256, ImGuiInputTextFlags_ReadOnly);
    if (!file_str.empty() && ImGui::BeginItemTooltip()) {
        ImGui::TextUnformatted(file_str.c_str());
        ImGui::EndTooltip();
    }

    const char* combo_items[] = {"uint8", "uint16", "uint32", "float32"};
    const int combo_values[] = {CV_8UC1, CV_16UC1, CV_32SC1, CV_32FC1};
    ImGui::PushItemWidth(width - ImGui::CalcTextSize("height").x - ImGui::GetStyle().ItemSpacing.x);
    ImGui::Combo("type", &_item_current, combo_items, IM_ARRAYSIZE(combo_items));
    ImGui::InputInt("offset", &config.offset);
    ImGui::InputInt("width", &config.width);
    ImGui::InputInt("height", &config.height);
    ImGui::InputInt("depth", &config.depth);
    ImGui::PopItemWidth();
    config.image_type = combo_values[_item_current];

    if (ImGui::Button("load")) {
        if (file_str.empty()) {
            ImGui::InsertNotification({ImGuiToastType::Warning, 3000, "Please select a file"});
        } else {
            status = NodeStatus::Processing;
            pool::enqueue([this]() {
                volume = imn::io::load_volume(
                    file_path, config,
                    [this](int progress, int max, const char* msg, std::any user_data) {
                        progress_cur = progress;
                        progress_max = max;
                    });
                status = NodeStatus::Dirty;
            });
        }
    }
}

ImagePreviewNode::ImagePreviewNode()
    : Node("Image Preview", ColorTheme::Orange), show_window(false) {
    in_image = std::make_shared<ImagePin>("image", PinKind::In);
    inputs[in_image->id] = in_image;
    _build_pins();

    status = NodeStatus::Pending;
}

void ImagePreviewNode::_draw_body() {
    // TODO: show image
}

void ImagePreviewNode::_process() {
    auto mat = get_input<std::shared_ptr<cv::Mat>>(in_image->id);
    if (!viewer_window) {
        viewer_widget = std::make_shared<ui::ImageWidget>();
        viewer_widget->show_toolbar(false);
        viewer_window = std::make_shared<WrapperWindow>(viewer_widget, "Image Preview");
        lambda::call("ADD_WINDOW", std::shared_ptr<BaseWindow>(viewer_window));
    }
    viewer_widget->set_image(mat);
    viewer_window->open();
}

VolumePreviewNode::VolumePreviewNode()
    : Node("Volume Preview", ColorTheme::Orange), show_window(false) {
    pin_vol = std::make_shared<VolumePin>("volume", PinKind::In);
    inputs[pin_vol->id] = pin_vol;
    _build_pins();

    status = NodeStatus::Pending;
}

void VolumePreviewNode::on_double_click() {
    if (viewer_window) {
        viewer_window->open();
    }
}

void VolumePreviewNode::_draw_body() {
    // auto size = ui::font_size();
    // viewer->show({size * 15, size * 15});
}

void VolumePreviewNode::_process() {
    auto mat = get_input<std::shared_ptr<cv::Mat>>(pin_vol->id);
    if (!viewer_window) {
        viewer_widget = std::make_shared<ui::VolumeWidget>();
        viewer_widget->set_node(this);
        viewer_window = std::make_shared<WrapperWindow>(viewer_widget, "Volume Preview");
        lambda::call("ADD_WINDOW", std::shared_ptr<BaseWindow>(viewer_window));
    }
    viewer_widget->set_volume(mat);
    viewer_window->open();
}
