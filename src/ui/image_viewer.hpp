
#ifndef UI__IMAGE_VIEWER_HPP
#define UI__IMAGE_VIEWER_HPP

#include <memory>

#include "base_window.hpp"

class Dtype;


class ImageViewer : public BaseWindow {
public:
    void set_image(std::shared_ptr<Dtype> image);
    void show() override;
private:
    void _create_toolbar();

    std::shared_ptr<Dtype> _image;
};

#endif
