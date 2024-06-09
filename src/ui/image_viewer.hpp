
#ifndef UI__IMAGE_VIEWER_HPP
#define UI__IMAGE_VIEWER_HPP

#include <memory>
#include <tuple>

#include <imgui.h>
#include <implot.h>
#include <opencv2/opencv.hpp>
#include "base_window.hpp"

enum MouseMode {
    kNone = 0,
    kRuler,
    kRect,
};

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
    ImVec2 _last_canvas_size;  // used to check if resize event happened
    ImVec2 _bounds_min;
    ImVec2 _bounds_max;
    double _scale;
    double _drag_x;
    double _drag_y;

    MouseMode _mouse_mode = kNone;
    double _ruler_points[4];  // {start_x, end_x, start_y, end_y}
    bool _show_horizontal_line;
};

#endif
