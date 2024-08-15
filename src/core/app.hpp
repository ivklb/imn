
#ifndef CORE__APP_HPP
#define CORE__APP_HPP

#include <memory>

namespace imn {

class App {
   public:
    static App* app();
    void init();
    void destroy();
};
}  // namespace imn

#endif
