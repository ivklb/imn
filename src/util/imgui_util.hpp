
#ifndef UTIL__IMGUI_UTIL_HPP
#define UTIL__IMGUI_UTIL_HPP

#include <string>
#include <imgui.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

#include "ext/stb/stb_image.h"


const ImWchar* GetGlyphRangesGreek();

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);

GLuint load_texture_2d(cv::Mat* img);
GLuint load_texture_2d(const char* filename);

bool ImageButtonWithText(ImTextureID texture, const std::string &text, float x, float y, float w, float h);


#endif
