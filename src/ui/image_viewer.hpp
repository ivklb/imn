
#ifndef UI__IMAGE_VIEWER_HPP
#define UI__IMAGE_VIEWER_HPP

#include <imgui.h>
#include <implot.h>

#include <map>
#include <memory>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <vector>

#include "base_window.hpp"

namespace imn::ui {

enum MouseMode {
    kNone = 0,
    kRuler,
    kRect,
};

class ImageViewer : public BaseWindow {
   public:
    ImageViewer();
    ~ImageViewer();
    void set_image(std::shared_ptr<cv::Mat> image);
    void set_images(std::vector<std::shared_ptr<cv::Mat>> images);
    void show() override;

   private:
    void _show_toolbar();
    void _show_image(ImVec2 region);
    std::tuple<ImVec2, ImVec2> _calc_paint_region(double image_width, double image_height, double canvas_width, double canvas_height);

    std::string _id;

    std::vector<std::shared_ptr<cv::Mat>> _images;
    std::map<int, ImTextureID> _tex_id_map;
    int _img_idx;

    ImVec2 _last_canvas_size;  // used to check if resize event happened
    ImVec2 _bounds_min;
    ImVec2 _bounds_max;
    double _scale;
    double _drag_x;
    double _drag_y;

    MouseMode _mouse_mode = kNone;
    double _ruler_points[4];  // {start_x, end_x, start_y, end_y}
    double _rect[4];          // {x_min, y_min, x_max, y_max}
    bool _show_horizontal_line;
};

}  // namespace imn::ui
#endif
