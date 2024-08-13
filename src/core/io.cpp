
#include "io.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <optional>
#include <regex>
#include <string>

namespace imn::io {

ImportConfig parse_filename(const std::string& file_path) {
    ImportConfig config = {0};

    // 1024x512x512
    // 1024_512_512
    // 1024-512-512
    // bar_2940x2304x721_ushort.raw
    // bar_in_helical_2022_11_01_09_18_27_Recon_1024x1024x1024.raw
    // bar_in_helical_2022_11_01_09_18_27_Recon_1024x1024x1024_uint32.raw
    std::regex pattern("(\\d+)[x_-](\\d+)[x_-](\\d+).{0,12}$");
    std::smatch matches;
    if (std::regex_search(file_path, matches, pattern)) {
        config.width = std::stoi(matches[1]);
        config.height = std::stoi(matches[2]);
        config.depth = std::stoi(matches[3]);
    }

    if (file_path.find("uint8") != std::string::npos) {
        config.image_type = CV_8UC1;
    } else if (file_path.find("uint16") != std::string::npos) {
        config.image_type = CV_16UC1;
    } else if (file_path.find("uint32") != std::string::npos) {
        config.image_type = CV_32SC1;
    } else if (file_path.find("float") != std::string::npos) {
        config.image_type = CV_32FC1;
    } else if (file_path.find("ushort") != std::string::npos) {
        config.image_type = CV_16UC1;
    }
    return config;
}

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
    auto file_size = std::filesystem::file_size(file_path);
    auto frame_size = size_t(config.height) * config.width * CV_ELEM_SIZE(config.image_type);
    auto available_frames = int((file_size - config.offset) / frame_size);
    available_frames = std::min(available_frames, config.depth);

    std::vector<int> dims = {available_frames, config.height, config.width};
    auto rv = std::make_shared<cv::Mat>(dims, config.image_type);

    std::ifstream infile(file_path, std::ios_base::binary);
    infile.seekg(config.offset, std::ios::beg);
    for (int k = 0; k < available_frames; ++k) {
        infile.read((char*)rv->data + k * frame_size, frame_size);
        if (cb) {
            cb(k, available_frames, "Loading volume", 0);
        }
    }
    return rv;
}

}  // namespace imn::io
