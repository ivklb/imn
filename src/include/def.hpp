
#ifndef INCLUDE__DEF_HPP
#define INCLUDE__DEF_HPP

#include <string>
#include <eigen3/Eigen/Dense>
#include <imgui.h>
#include <memory>

#ifdef IMN_EXPORT
#  define IMN_API extern "C" __declspec(dllexport)
#else
#  define IMN_API extern "C" __declspec(dllimport)
#endif


typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> Dtype;

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

#endif
