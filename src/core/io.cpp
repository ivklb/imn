
#include "io.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

namespace imn::io {

bool is_image(const std::filesystem::path& file_path) {
    std::string ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext.ends_with(".png") || ext.ends_with(".bmp") ||
           ext.ends_with(".jpeg") || ext.ends_with(".jpg") ||
           ext.ends_with(".tiff") || ext.ends_with(".tif");
}

std::shared_ptr<cv::Mat>
load_image(const std::filesystem::path& file_path, ImportConfig config) {
    if (is_image(file_path)) {
        if (!std::filesystem::exists(file_path)) {
            SPDLOG_DEBUG("file not exist: {}", file_path.string());
        }
        std::ifstream infile(file_path.string(), std::ios_base::binary);
        std::vector<char> buffer{std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
        cv::Mat mat = cv::imdecode(buffer, cv::IMREAD_UNCHANGED);
        return std::make_shared<cv::Mat>(std::move(mat));
    } else {
        auto rv = std::make_shared<cv::Mat>(config.height, config.width, config.image_type);
        auto mat_size = rv->rows * rv->cols * rv->elemSize();
        auto file_size = std::filesystem::file_size(file_path);
        auto read_size = std::min(mat_size, file_size);

        std::ifstream infile(file_path, std::ios_base::binary);
        infile.seekg(config.offset, std::ios::beg);
        infile.read((char*)rv->data, read_size);
        return rv;
    }
}

std::vector<std::shared_ptr<cv::Mat>>
load_image_stack(const std::vector<std::string>& file_list, ImportConfig config) {
    std::vector<std::shared_ptr<cv::Mat>> images;
    for (auto const& filename : file_list) {
        images.push_back(load_image(filename, config));
    }
    return images;
}

std::vector<std::shared_ptr<cv::Mat>>
load_image_stack(const std::vector<std::filesystem::path>& file_list, ImportConfig config, ProgressCallback cb) {
    std::vector<std::shared_ptr<cv::Mat>> images;
    int progress = 0;
    if (cb) {
        cb(progress++, file_list.size(), "Loading images", 0);
    }
    for (auto const& filename : file_list) {
        images.push_back(load_image(filename, config));
        if (cb) {
            cb(progress++, file_list.size(), "Loading images", 0);
        }
    }
    if (cb) {
        cb(file_list.size(), file_list.size(), "Loading images done", 0);
    }
    return images;
}

std::shared_ptr<cv::Mat>
load_volume(const std::filesystem::path& file_path, ImportConfig config, ProgressCallback cb) {
    std::vector<int> dims = {config.depth, config.height, config.width};
    auto rv = std::make_shared<cv::Mat>(dims, config.image_type);
    auto mat_size = rv->total() * rv->elemSize();
    auto file_size = std::filesystem::file_size(file_path);
    auto read_size = std::min(mat_size, file_size);

    std::ifstream infile(file_path, std::ios_base::binary);
    infile.seekg(config.offset, std::ios::beg);
    // for ()
    infile.read((char*)rv->data, read_size);
    return rv;
}

}  // namespace imn::io
