

#include "nodes.hpp"

#include <spdlog/spdlog.h>
#include <utf8.h>
#include <utf8/cpp20.h>

#include <algorithm>
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

DemoNode::DemoNode(const char* name, ColorTheme color) : Node() {
    auto pa = std::make_shared<IntPin>(this, "a", PinKind::In, ColorTheme::Green);
    auto pb = std::make_shared<IntPin>(this, "b", PinKind::In, ColorTheme::Red);
    auto pc = std::make_shared<IntPin>(this, "c", PinKind::Out, ColorTheme::Blue);

    inputs[pa->name] = pa;
    inputs[pb->name] = pb;
    outputs[pc->name] = pc;

    status = NodeStatus::Processing;
}

UnaryOperatorNode::UnaryOperatorNode() : Node(), selected_op(0), old_selected_op(0) {
    auto p_in = std::make_shared<ImagePin>(this, "mat", PinKind::In);
    auto p_out = std::make_shared<ImagePin>(this, "mat", PinKind::Out);
    inputs[p_in->name] = p_in;
    outputs[p_out->name] = p_out;
}

void UnaryOperatorNode::fit_json(json data) {
    Node::fit_json(data);
    selected_op = data.value("op", 0);
    old_selected_op = selected_op;
}

json UnaryOperatorNode::to_json() {
    json rv = Node::to_json();
    rv["op"] = selected_op;
    return rv;
}

float UnaryOperatorNode::width() {
    return ui::get_style().font_size * 8.0f;
}

std::any UnaryOperatorNode::get_output(int pid) {
    assert(status == NodeStatus::Done && "UnaryOperatorNode::get_output() called before processing is done");
    return result;
}

void UnaryOperatorNode::_draw_body() {
    static const std::map<int, std::string> op_names = {
        {int(Operator::Negation), "Negation"},
        {int(Operator::Log), "Log"},
        {int(Operator::Transpose), "Transpose"},
        {int(Operator::FlipLR), "FlipLR"},
        {int(Operator::FlipUD), "FlipUD"},
        {int(Operator::Rotate90), "Rotate90"},
        {int(Operator::Rotate180), "Rotate180"},
        {int(Operator::Rotate270), "Rotate270"},
    };

    ImGui::PushItemWidth(width() - ImGui::CalcTextSize("height").x - ImGui::GetStyle().ItemSpacing.x);
    ImGui::Combo("operator", &selected_op, [](void* data, int n) { return op_names.at(n).c_str(); }, nullptr, int(op_names.size()));
    ImGui::PopItemWidth();

    if (selected_op != old_selected_op) {
        status = NodeStatus::Pending;
        old_selected_op = selected_op;
    }
}

void UnaryOperatorNode::_process() {
    auto op_ = static_cast<Operator>(selected_op);
    auto in_mat = get_input<std::shared_ptr<cv::Mat>>("mat");
    auto rv = std::make_shared<cv::Mat>();

    if (op_ == Operator::Negation) {
        in_mat->convertTo(*rv, CV_32F);
        *rv = -(*rv);
    } else if (op_ == Operator::Log) {
        in_mat->convertTo(*rv, CV_32F);
        size_t count = rv->total() * rv->channels();
        std::for_each(
            (float*)(rv->data),
            (float*)(rv->data) + count,
            [](float& x) { x = std::log(x); });
    } else if (op_ == Operator::Transpose) {
        *rv = in_mat->t();
    } else if (op_ == Operator::FlipLR) {
        cv::flip(*in_mat, *rv, 1);
    } else if (op_ == Operator::FlipUD) {
        cv::flip(*in_mat, *rv, 0);
    } else if (op_ == Operator::Rotate90) {
        cv::rotate(*in_mat, *rv, cv::ROTATE_90_CLOCKWISE);
    } else if (op_ == Operator::Rotate180) {
        cv::rotate(*in_mat, *rv, cv::ROTATE_180);
    } else if (op_ == Operator::Rotate270) {
        cv::rotate(*in_mat, *rv, cv::ROTATE_90_COUNTERCLOCKWISE);
    }
    result = rv;
}

ImageLoaderNode::ImageLoaderNode() : Node(), config({}), _item_current(0) {
    auto p = std::make_shared<ImagePin>(this, "image", PinKind::Out);
    outputs[p->name] = p;
}

void ImageLoaderNode::fit_json(json data) {
    Node::fit_json(data);
    file_str = data.value("file_str", "");
    file_path = utf8::utf8to16(file_str);
    config.offset = data.value("config.offset", 0);
    config.width = data.value("config.width", 0);
    config.height = data.value("config.height", 0);
    config.image_type = data.value("config.image_type", CV_8UC1);
}

json ImageLoaderNode::to_json() {
    json rv = Node::to_json();
    rv["file_str"] = file_str;
    rv["config.offset"] = config.offset;
    rv["config.width"] = config.width;
    rv["config.height"] = config.height;
    rv["config.image_type"] = config.image_type;
    return rv;
}

std::any ImageLoaderNode::get_output(int pid) {
    assert(status == NodeStatus::Done && "ImageLoaderNode::get_output() called before processing is done");
    return image;
}

void ImageLoaderNode::_draw_body() {
    auto button_width = (width() - ImGui::GetStyle().ItemSpacing.x) * 0.3f;
    auto text_width = (width() - ImGui::GetStyle().ItemSpacing.x) * 0.7f;

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
        ImGui::PushItemWidth(width() - ImGui::CalcTextSize("height").x - ImGui::GetStyle().ItemSpacing.x);
        ImGui::Combo("type", &_item_current, combo_items, IM_ARRAYSIZE(combo_items));
        ImGui::InputInt("offset", &config.offset);
        ImGui::InputInt("width", &config.width);
        ImGui::InputInt("height", &config.height);
        ImGui::PopItemWidth();
        config.image_type = combo_values[_item_current];
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

VolumeLoaderNode::VolumeLoaderNode() : Node(), config({}) {
    auto p = std::make_shared<VolumePin>(this, "volume", PinKind::Out);
    outputs[p->name] = p;

    _item_current = 0;
}

void VolumeLoaderNode::fit_json(json data) {
    Node::fit_json(data);
    file_str = data.value("file_str", "");
    file_path = utf8::utf8to16(file_str);
    config.offset = data.value("config.offset", 0);
    config.width = data.value("config.width", 0);
    config.height = data.value("config.height", 0);
    config.depth = data.value("config.depth", 0);
    _item_current = data.value("config.selected_type", 0);
}

json VolumeLoaderNode::to_json() {
    json rv = Node::to_json();
    rv["file_str"] = file_str;
    rv["config.offset"] = config.offset;
    rv["config.width"] = config.width;
    rv["config.height"] = config.height;
    rv["config.depth"] = config.depth;
    rv["config.selected_type"] = _item_current;
    return rv;
}

std::any VolumeLoaderNode::get_output(int pid) {
    assert(status == NodeStatus::Done && "VolumeLoaderNode::get_output() called before processing is done");
    return volume;
}

void VolumeLoaderNode::_draw_body() {
    auto width = ui::get_style().font_size * 8.0f;
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

ImagePreviewNode::ImagePreviewNode() : Node(), show_window(false) {
    in_image = std::make_shared<ImagePin>(this, "image", PinKind::In);
    inputs[in_image->name] = in_image;
}

void ImagePreviewNode::_draw_body() {
    // TODO: show image
}

void ImagePreviewNode::_process() {
    auto mat = get_input<std::shared_ptr<cv::Mat>>(in_image->id);
    if (!viewer_window) {
        viewer_widget = std::make_shared<ui::ImageWidget>();
        viewer_widget->set_node(this);
        viewer_widget->show_toolbar(false);
        viewer_window = std::make_shared<WrapperWindow>(viewer_widget, "Image Preview");
        lambda::call("ADD_WINDOW", std::shared_ptr<BaseWindow>(viewer_window));
    }
    viewer_widget->set_image(mat);
    viewer_window->open();
}

VolumePreviewNode::VolumePreviewNode() : Node(), show_window(false) {
    pin_vol = std::make_shared<VolumePin>(this, "volume", PinKind::In);
    inputs[pin_vol->name] = pin_vol;
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
