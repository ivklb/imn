
#include "imgui_util.hpp"

#include <fstream>
#include <imgui.h>

#define STB_IMAGE_IMPLEMENTATION
#include "ext/stb/stb_image.h"

#include <spdlog/spdlog.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>


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

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

GLuint load_texture_2d(cv::Mat* img) {
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
    return image_texture;
}

GLuint load_texture_2d(const char* filename) {
    auto img = cv::imread(filename, cv::IMREAD_UNCHANGED);
    return load_texture_2d(&img);
}

bool ImageButtonWithText(ImTextureID texture, const std::string& text, float x, float y, float w, float h) {
    ImGui::SetCursorPos({ x, y });
    bool clicked = ImGui::ImageButton(("Button_" + text).c_str(), texture, { w, h });
    ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
    ImGui::SetCursorPos({ x + (w - text_size.x) * 0.5f, y + (h - text_size.y) * 0.5f });
    ImGui::Text(text.c_str());
    return clicked;
}
