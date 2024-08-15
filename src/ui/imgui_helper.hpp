
#ifndef UI__IMGUI_HELPER_HPP
#define UI__IMGUI_HELPER_HPP

#include <imgui.h>

#include <memory>
#include <string>

#include "ui/base_window.hpp"

namespace imn::ui {

class IDGenerator {
   public:
    static int next();
    static void set_next(int id);

   private:
    static int _next_id;
};

class WrapperWindow : public BaseWindow {
   public:
    WrapperWindow(std::shared_ptr<BaseWidget> widget, const std::string& title = {}, const ImVec2& size = ImVec2(0, 0));
    void show(ImVec2 size = ImVec2(0, 0)) override;

   private:
    std::shared_ptr<BaseWidget> _widget;
    int _id;
};

struct Style {
    float font_size;
    ImVec2 image_button_size;
};

Style& get_style();

float font_size();

const ImWchar* GetGlyphRangesGreek();

bool ImageButton(
    const char* str_id,
    const std::string& image_file,
    const bool active = false,
    const ImVec2& image_size = ImVec2(60, 60),
    const ImVec2& uv0 = ImVec2(0, 0),
    const ImVec2& uv1 = ImVec2(1, 1),
    const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
    const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

bool ImageButtonWithText(ImTextureID texture, const std::string& text, float x, float y, float w, float h);

}  // namespace imn::ui

#endif
