
#ifndef UI__NODES__NODES_HPP
#define UI__NODES__NODES_HPP

#include <imgui.h>

#include <filesystem>
#include <opencv2/opencv.hpp>

#include "core/io.hpp"
#include "core/object.hpp"
#include "ui/imgui_helper.hpp"
#include "ui/nodes/struct.hpp"
#include "ui/widgets/image_widget.hpp"
#include "ui/widgets/volume_widget.hpp"

namespace imn::ui {

struct DemoNode : public Node {
    DemoNode(const char* name, ColorTheme color = ColorTheme::Blue);
};

struct UnaryOperatorNode : public Node, public core::AutoRegistered<UnaryOperatorNode, Node> {
    enum class Operator {
        Negation,
        Log,
        Transpose,
        FlipLR,
        FlipUD,
        Rotate90,
        Rotate180,
        Rotate270,
    };

    std::shared_ptr<cv::Mat> input_mat;
    std::shared_ptr<cv::Mat> result;
    int selected_op;
    int old_selected_op;

    UnaryOperatorNode();
    void fit_json(json data) override;
    json to_json() override;
    static std::string registered_name() { return "unary_operator"; }
    std::string name() override { return registered_name(); }
    ColorTheme color() override { return ColorTheme::Green; }
    float width() override;
    std::any get_output(int pid) override;
    void _draw_body() override;
    void _process() override;
};

struct ImageLoaderNode : public Node, public core::AutoRegistered<ImageLoaderNode, Node> {
    std::string file_str;             // used to display in the UI
    std::filesystem::path file_path;  // used to access file with unicode path
    imn::io::ImportConfig config;
    std::shared_ptr<cv::Mat> image;
    int _item_current;

    ImageLoaderNode();
    void fit_json(json data) override;
    json to_json() override;
    static std::string registered_name() { return "image_loader"; }
    std::string name() override { return registered_name(); }
    ColorTheme color() override { return ColorTheme::Red; }
    std::any get_output(int pid) override;
    void _draw_body() override;
};

struct VolumeLoaderNode : public Node, public core::AutoRegistered<VolumeLoaderNode, Node> {
    std::string file_str;             // used to display in the UI
    std::filesystem::path file_path;  // used to access file with unicode path
    imn::io::ImportConfig config;
    std::shared_ptr<cv::Mat> volume;
    int _item_current;

    VolumeLoaderNode();
    void fit_json(json data) override;
    json to_json() override;
    static std::string registered_name() { return "volume_loader"; }
    std::string name() override { return registered_name(); }
    ColorTheme color() override { return ColorTheme::Red; }
    std::any get_output(int pid) override;
    void _draw_body() override;
};

struct ImagePreviewNode : public Node, public core::AutoRegistered<ImagePreviewNode, Node> {
    std::shared_ptr<Pin> in_image;
    bool show_window;
    std::shared_ptr<ImageWidget> viewer_widget;
    std::shared_ptr<WrapperWindow> viewer_window;

    ImagePreviewNode();
    static std::string registered_name() { return "image_preview"; }
    std::string name() override { return registered_name(); }
    ColorTheme color() override { return ColorTheme::Orange; }
    void _draw_body() override;
    void _process() override;
};

struct VolumePreviewNode : public Node, public core::AutoRegistered<VolumePreviewNode, Node> {
    std::shared_ptr<Pin> pin_vol;
    bool show_window;
    std::shared_ptr<VolumeWidget> viewer_widget;
    std::shared_ptr<WrapperWindow> viewer_window;

    VolumePreviewNode();
    static std::string registered_name() { return "volume_preview"; }
    std::string name() override { return registered_name(); }
    ColorTheme color() override { return ColorTheme::Orange; }
    void on_double_click() override;
    void _draw_body() override;
    void _process() override;
};

}  // namespace imn::ui

#endif
