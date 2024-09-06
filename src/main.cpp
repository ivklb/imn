

#include "core/app.hpp"
#include "ui/main_window.hpp"

int main(int argc, char* argv[]) {
    imn::app::init();

    imn::ui::MainWindow window;
    window.show();

    imn::app::stop();
    return 0;
}
