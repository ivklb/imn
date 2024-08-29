
#include "app.hpp"

#include "core/log.hpp"
#include "core/python.hpp"
#include "include/def.hpp"

using namespace imn;

namespace imn::app {
static bool running = true;

void init() {
    log::init();
    python::init();
}

bool is_running() {
    return running;
}

void stop() {
    running = false;
}

}  // namespace imn::app