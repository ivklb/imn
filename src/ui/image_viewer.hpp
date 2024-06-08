
#ifndef UI__IMAGE_VIEWER_HPP
#define UI__IMAGE_VIEWER_HPP

#include <memory>

#include <opencv2/opencv.hpp>
#include "base_window.hpp"


class ImageViewer : public BaseWindow {
public:
    void set_image(std::shared_ptr<cv::Mat> image);
    void show() override;

private:
    void _show_toolbar();
    void _show_image();

    std::shared_ptr<cv::Mat> _image;
    void* _tex_id;
};

#endif
