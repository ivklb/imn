

#include "ui/main_window.hpp"
#include "core/app.hpp"


int main(int argc, char* argv[]) {
    // trigger init
    Moon::App::app();

    Moon::ui::MainWindow window;
    window.show();
    return 0;
}
