

#include "ui/main_window.hpp"
#include "core/moon.hpp"


int main(int argc, char* argv[]) {
    Moon moon;
    moon.init();

    MainWindow window;
    window.show();
    return 0;
}
