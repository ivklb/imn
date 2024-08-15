
#ifndef UI__BASE_WINDOW_HPP
#define UI__BASE_WINDOW_HPP

#include <imgui.h>

#include <string>

namespace imn::ui {

class BaseWidget {
   public:
    virtual ~BaseWidget() {}
    virtual void show(ImVec2 size = ImVec2(0, 0)) {}

   protected:
   private:
};

class BaseWindow {
   public:
    virtual ~BaseWindow() {}
    virtual void show(ImVec2 size = ImVec2(0, 0)) {}

   protected:
   private:
};

}  // namespace imn::ui
#endif
