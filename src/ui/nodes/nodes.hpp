
#ifndef UI__NODES__NODES_HPP
#define UI__NODES__NODES_HPP

#include <imgui.h>

#include <filesystem>
#include <opencv2/opencv.hpp>

#include "core/io.hpp"
#include "ui/imgui_helper.hpp"
#include "ui/nodes/struct.hpp"
#include "ui/widgets/image_widget.hpp"
#include "ui/widgets/volume_widget.hpp"

namespace imn::ui {

struct DemoNode : public Node {
    DemoNode(const char* name, ColorTheme color = ColorTheme::Blue);
};

struct ImageLoaderNode : public Node {
    std::string file_str;             // used to display in the UI
    std::filesystem::path file_path;  // used to access file with unicode path
    imn::io::ImportConfig config;
    std::shared_ptr<cv::Mat> image;

    ImageLoaderNode();
    std::string name() override { return "Image Loader"; }
    ColorTheme color() override { return ColorTheme::Red; }
    std::any get_output(int pid) override;
    void _draw_body() override;
};

struct VolumeLoaderNode : public Node {
    std::string file_str;             // used to display in the UI
    std::filesystem::path file_path;  // used to access file with unicode path
    imn::io::ImportConfig config;
    std::shared_ptr<cv::Mat> volume;
    int _item_current;

    VolumeLoaderNode();
    std::string name() override { return "Volume Loader"; }
    ColorTheme color() override { return ColorTheme::Red; }
    std::any get_output(int pid) override;
    void _draw_body() override;
};

struct ImagePreviewNode : public Node {
    std::shared_ptr<Pin> in_image;
    bool show_window;
    std::shared_ptr<ImageWidget> viewer_widget;
    std::shared_ptr<WrapperWindow> viewer_window;

    ImagePreviewNode();
    std::string name() override { return "Image Preview"; }
    ColorTheme color() override { return ColorTheme::Orange; }
    void _draw_body() override;
    void _process() override;
};

struct VolumePreviewNode : public Node {
    std::shared_ptr<Pin> pin_vol;
    bool show_window;
    std::shared_ptr<VolumeWidget> viewer_widget;
    std::shared_ptr<WrapperWindow> viewer_window;

    VolumePreviewNode();
    std::string name() override { return "Volume Preview"; }
    ColorTheme color() override { return ColorTheme::Orange; }
    void on_double_click() override;
    void _draw_body() override;
    void _process() override;
};

}  // namespace imn::ui

#endif
