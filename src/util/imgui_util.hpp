
#ifndef UTIL__IMGUI_UTIL_HPP
#define UTIL__IMGUI_UTIL_HPP

#include <string>
#include <imgui.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>


const ImWchar* GetGlyphRangesGreek();
float get_font_size();
float get_input_box_height();
ImTextureID load_texture_2d(const std::string& img, bool nearest_sample = true);
ImTextureID load_texture_2d(const cv::Mat* img, bool nearest_sample = true);


#endif
