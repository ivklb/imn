
#ifndef UI__BASE_WINDOW_HPP
#define UI__BASE_WINDOW_HPP

#include <imgui.h>

#include <functional>
#include <memory>
#include <string>

class BaseWindow {
   public:
    virtual ~BaseWindow() {}
    virtual void show(ImVec2 size = ImVec2(0, 0)) {}

   protected:
   private:
};

#endif
