
#ifndef INCLUDE__DEF_HPP
#define INCLUDE__DEF_HPP

#include <string>

#ifdef MOON_EXPORT
#  define MOON_API extern "C" __declspec(dllexport)
#else
#  define MOON_API extern "C" __declspec(dllimport)
#endif


const std::string kAppName = "moon";
const std::string kAsciiLogo = R"(
  __  __
 |  \/  |
 | \  / |   ___     ___    _ __
 | |\/| |  / _ \   / _ \  | '_ \
 | |  | | | (_) | | (_) | | | | |
 |_|  |_|  \___/   \___/  |_| |_|)";


#endif
