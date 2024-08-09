
#ifndef UTIL__IMGUI_UTIL_HPP
#define UTIL__IMGUI_UTIL_HPP

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include <opencv2/opencv.hpp>
#include <string>

const ImWchar* GetGlyphRangesGreek();
ImTextureID load_texture_2d(const std::string& img, bool nearest_sample = true);
ImTextureID load_texture_2d(const cv::Mat* img, bool nearest_sample = true);

#endif
