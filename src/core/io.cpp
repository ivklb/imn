
#include "io.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>


namespace Moon {
namespace IO {

std::vector<std::shared_ptr<cv::Mat>>
load_image_stack(const std::vector<std::string>& file_list, ImportConfig config) {
    std::vector<std::shared_ptr<cv::Mat>> images;
    if (config.common_image) {
        for (auto const& filename : file_list) {
            auto img = cv::imread(filename, cv::IMREAD_UNCHANGED);
            images.push_back(std::make_shared<cv::Mat>(std::move(img)));
            // TODO: add progress
        }
    } else {
        for (auto const& filename : file_list) {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);
            // TODO: fix me
        }
    }
    return images;
}

}
}
