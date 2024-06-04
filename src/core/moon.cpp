
#include "moon.hpp"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "include/def.hpp"


void Moon::init() {
    // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
    spdlog::set_pattern("%L %m-%d %T.%e %s:%# %! %v");
    spdlog::flush_every(std::chrono::seconds(1));
    auto max_size = 50 * 1024 * 1024;  // 50MB
    auto max_files = 10;
    auto logger = spdlog::rotating_logger_mt(kAppName, "log/rotating.txt", max_size, max_files);
    spdlog::set_default_logger(logger);
    SPDLOG_INFO(kAsciiLogo);
}
