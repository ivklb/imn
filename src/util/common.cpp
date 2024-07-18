
#include "common.hpp"

#include <wtypes.h>


int get_unique_id() {
    static int id = 0;
    return id++;
}

std::tuple<int, int> get_screen_resolution() {
    // SetProcessDPIAware();
    const HWND hDesktop = GetDesktopWindow();

    RECT desktop;
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    return { desktop.right, desktop.bottom };
}
