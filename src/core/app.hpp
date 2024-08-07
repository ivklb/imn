
#ifndef CORE__MOON_HPP
#define CORE__MOON_HPP

#include <GLFW/glfw3.h>

#include <memory>

namespace Moon {

class App {
   public:
    static App* app();
    void init();
    void destroy();
    GLFWwindow* main_window_handle() { return _window; }
    void set_main_window_handle(GLFWwindow* w) { _window = w; }

   private:
    GLFWwindow* _window;
};
}  // namespace Moon

#endif
