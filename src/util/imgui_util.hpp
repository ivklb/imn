
#ifndef UTIL__IMGUI_UTIL_HPP
#define UTIL__IMGUI_UTIL_HPP

#include <imgui.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "ext/stb/stb_image.h"


const ImWchar* GetGlyphRangesGreek();

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);

#endif
