
#ifndef UTIL__IMGUI_UTIL_HPP
#define UTIL__IMGUI_UTIL_HPP

#include <string>
#include <imgui.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>


const ImWchar* GetGlyphRangesGreek();

ImTextureID load_texture_2d(const std::string& img);
ImTextureID load_texture_2d(const cv::Mat* img);


#endif
