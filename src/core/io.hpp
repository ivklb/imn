
#ifndef CORE__IO_HPP
#define CORE__IO_HPP

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <opencv2/core.hpp>
#include <string>
#include <vector>

#include "include/def.hpp"

namespace imn::io {

struct ImportConfig {
    bool common_image;  // png/jpg/tiff/...
    int image_type;
    int width;
    int height;
    int depth;
    int offset;
    int num_images;
    bool little_endian;
};

bool is_image(const std::filesystem::path& file_path);

std::shared_ptr<cv::Mat>
load_image(const std::filesystem::path& file_path, ImportConfig config);

std::vector<std::shared_ptr<cv::Mat>>
load_image_stack(
    const std::vector<std::string>& file_list,
    ImportConfig config);

std::vector<std::shared_ptr<cv::Mat>>
load_image_stack(
    const std::vector<std::filesystem::path>& file_list,
    ImportConfig config, ProgressCallback callback = nullptr);

std::shared_ptr<cv::Mat>
load_volume(const std::filesystem::path& file_path, ImportConfig config, ProgressCallback callback = nullptr);

}  // namespace imn::io

#endif
