
#ifndef UI__NODES__NODES_HPP
#define UI__NODES__NODES_HPP

#include <imgui.h>

#include <filesystem>
#include <opencv2/opencv.hpp>

#include "core/io.hpp"
#include "ui/image_viewer.hpp"
#include "ui/nodes/struct.hpp"

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
    std::any get_output(int pid) override;
    void _draw_body() override;
};

struct ImagePreviewNode : public Node {
    std::shared_ptr<Pin> in_image;
    bool show_window;
    std::shared_ptr<ImageViewer> viewer;

    ImagePreviewNode();
    void _draw_body() override;
    void _process() override;
};

struct VolumePreviewNode : public Node {
    std::shared_ptr<Pin> in_image;
    bool show_window;
    std::shared_ptr<ImageViewer> viewer;

    VolumePreviewNode();
    void _draw_body() override;
    void _process() override;
};

}  // namespace imn::ui

#endif
