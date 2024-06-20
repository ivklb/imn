
#ifndef CORE__IO_HPP
#define CORE__IO_HPP

#include <fstream>
#include <iostream>
#include <memory>
#include <opencv2/core.hpp>
#include <string>
#include <vector>


namespace Moon {
namespace IO {

struct ImportConfig {
    bool common_image;  // png/jpg/tiff/...
    int image_type;
    int width;
    int height;
    int offset;
    int num_images;
    bool little_endian;
};

std::vector<std::shared_ptr<cv::Mat>>
load_image_stack(
    const std::vector<std::string>& file_list,
    ImportConfig config);
}
}

#endif
