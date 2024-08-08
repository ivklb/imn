

#include "ui/main_window.hpp"
#include "core/app.hpp"


int main(int argc, char* argv[]) {
    // trigger init
    imn::App::app();

    imn::ui::MainWindow window;
    window.show();
    return 0;
}
