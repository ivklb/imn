
#ifndef UI__NODES__NODES_HPP
#define UI__NODES__NODES_HPP

#include <imgui.h>

#include <opencv2/opencv.hpp>

#include "ui/nodes/struct.hpp"

namespace Moon::ui {

struct DemoNode : public Node {
    DemoNode(const char* name, ColorTheme color = ColorTheme::Blue);
};

struct ImageOpenNode : public Node {
    cv::Mat image;

    ImageOpenNode();
};

}  // namespace Moon::ui

#endif
