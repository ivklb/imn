
#ifndef INCLUDE__DEF_HPP
#define INCLUDE__DEF_HPP

#include <imgui.h>

#include <any>
#include <eigen3/Eigen/Dense>
#include <functional>
#include <memory>
#include <string>

#ifdef IMN_EXPORT
#define IMN_API extern "C" __declspec(dllexport)
#else
#define IMN_API extern "C" __declspec(dllimport)
#endif

const std::string kAppName = "imn";
const std::string kAsciiLogo = R"(
 /$$
|__/
 /$$ /$$$$$$/$$$$  /$$$$$$$
| $$| $$_  $$_  $$| $$__  $$
| $$| $$ \ $$ \ $$| $$  \ $$
| $$| $$ | $$ | $$| $$  | $$
| $$| $$ | $$ | $$| $$  | $$
|__/|__/ |__/ |__/|__/  |__/)";

const std::string kDialogChooseFile = "dialog_import_file";
const std::string kDialogImportFile = "dialog_choose_file";

const ImVec4 kColorWhite = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
const ImVec4 kColorBlack = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
const ImVec4 kColorRed = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
const ImVec4 kColorGreen = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
const ImVec4 kColorBlue = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
const ImVec4 kColorYellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

typedef std::function<void(int progress, int max, const char* msg, std::any user_data)> ProgressCallback;

#endif
