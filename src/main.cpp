

#include "ui/main_window.hpp"
#include "core/app.hpp"


int main(int argc, char* argv[]) {
    Moon::App app;
    app.init();

    Moon::ui::MainWindow window;
    window.show();
    return 0;
}
