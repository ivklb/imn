
#include "moon.hpp"

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "include/def.hpp"


void Moon::init() {
    auto max_size = 50 * 1024 * 1024;  // 50MB
    auto max_files = 10;
    auto logger = spdlog::rotating_logger_mt("moon", "log/rotating.txt", max_size, max_files);
    spdlog::set_default_logger(logger);

    spdlog::info(kAsciiLogo);
}
