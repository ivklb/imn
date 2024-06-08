
#ifndef UI__IMAGE_VIEWER_HPP
#define UI__IMAGE_VIEWER_HPP

#include <memory>
#include <tuple>

#include <imgui.h>
#include <opencv2/opencv.hpp>
#include "base_window.hpp"


class ImageViewer : public BaseWindow {
public:
    void set_image(std::shared_ptr<cv::Mat> image);
    void show() override;

private:
    void _show_toolbar();
    void _show_image();
    std::tuple<ImVec2, ImVec2> _calc_paint_region(double image_width, double image_height, double canvas_width, double canvas_height);

    std::shared_ptr<cv::Mat> _image;
    ImTextureID _tex_id;
};

#endif
