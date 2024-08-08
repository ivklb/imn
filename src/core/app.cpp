
#include "app.hpp"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "include/def.hpp"

using namespace imn;


App* App::app() {
    static bool init_flag = false;
    static App _app;
    if (!init_flag) {
        _app.init();
        init_flag = true;
    }
    return &_app;
}

void App::init() {
    auto max_size = 50 * 1024 * 1024;  // 50MB
    auto max_files = 10;
    auto logger = spdlog::rotating_logger_mt(kAppName, "log/rotating.txt", max_size, max_files);
    // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
    logger->set_pattern("%L %m-%d %T.%e %s:%# %v");
    logger->flush_on(spdlog::level::trace);
#ifdef NDEBUG
    logger->set_level(spdlog::level::info);
#else
    logger->set_level(spdlog::level::trace);
#endif
    spdlog::set_default_logger(logger);
    SPDLOG_INFO(kAsciiLogo);
}
