
#include "imgui_util.hpp"

#include <fstream>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "core/cache.hpp"

using namespace Moon;


const ImWchar* GetGlyphRangesGreek() {
    static const ImWchar ranges[] = {
        0x0020,
        0x00FF, // Basic Latin + Latin Supplement
        0x0370,
        0x03FF, // Greek and Coptic
        0,
    };
    return &ranges[0];
}

ImTextureID load_texture_2d(const std::string& img) {
    if (Cache::has(img)) {
        return Cache::get<ImTextureID>(img);
    }

    auto mat = cv::imread(img, cv::IMREAD_UNCHANGED);
    auto rv = load_texture_2d(&mat);
    Cache::add(img, rv);
    return rv;
}

ImTextureID load_texture_2d(const cv::Mat* img) {
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    // https://stackoverflow.com/questions/16809833/opencv-image-loading-for-opengl-texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, (img->step & 3) ? 1 : 4);

    if (img->type() == CV_8UC1) {
        // TODO: review me
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, img->cols, img->rows, 0, GL_RED, GL_UNSIGNED_BYTE, img->data);
    } else if (img->type() == CV_8UC3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->cols, img->rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img->data);
    } else if (img->type() == CV_16UC1) {
        // TODO: review me
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, img->cols, img->rows, 0, GL_RED, GL_UNSIGNED_SHORT, img->data);
    } else if (img->type() == CV_32FC1) {
        // TODO: review me
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, img->cols, img->rows, 0, GL_RED, GL_FLOAT, img->data);
    } else {
        SPDLOG_ERROR("unsupported image type {}", img->type());
    }
    return (ImTextureID)image_texture;
}
