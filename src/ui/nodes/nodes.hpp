
#ifndef UI__NODES__NODES_HPP
#define UI__NODES__NODES_HPP

#include <imgui.h>

#include <filesystem>
#include <opencv2/opencv.hpp>

#include "core/io.hpp"
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
    void _draw_body() override;
};

struct ImagePreviewNode : public Node {
    std::shared_ptr<Pin> in_image;

    ImagePreviewNode();
    void _draw_body() override;
    void _process() override;
};

}  // namespace imn::ui

#endif
