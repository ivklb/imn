
#ifndef UI__NODES__NODES_HPP
#define UI__NODES__NODES_HPP

#include <imgui.h>

#include <filesystem>
#include <opencv2/opencv.hpp>

#include "ui/nodes/struct.hpp"

namespace imn::ui {

struct DemoNode : public Node {
    DemoNode(const char* name, ColorTheme color = ColorTheme::Blue);
};

struct ImageLoaderNode : public Node {
    std::shared_ptr<cv::Mat> image;
    std::string file_path;

    ImageLoaderNode();
    void _draw_body() override;
};

struct ImagePreviewNode : public Node {
    ImagePreviewNode();
    void _draw_body() override;
};


}  // namespace imn::ui

#endif
