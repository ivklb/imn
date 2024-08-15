
#ifndef CORE__BACKEND_HPP
#define CORE__BACKEND_HPP

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include <functional>
#include <opencv2/opencv.hpp>

namespace imn::backend {

inline GLFWwindow* window;

void setup();
void cleanup();
void show(std::function<void()> frame_callback);

ImTextureID load_texture_2d(const std::string& img, bool nearest_sample = true);
ImTextureID load_texture_2d(const cv::Mat* img, bool nearest_sample = true);

}  // namespace imn::backend

#endif