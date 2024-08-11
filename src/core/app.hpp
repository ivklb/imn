
#ifndef CORE__APP_HPP
#define CORE__APP_HPP

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <memory>

namespace imn {

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
}  // namespace imn

#endif
