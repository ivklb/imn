#include "fs.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
// #include <shellapi.h>
#include <shlobj.h>
#include <windows.h>

#include <ext/nfd/src/include/nfd.hpp>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <ext/nfd/src/include/nfd_glfw3.h>
#undef GLFW_EXPOSE_NATIVE_WIN32

#include <fmt/format.h>
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include "core/backend.hpp"

namespace fs = std::filesystem;

namespace imn::fs {

// With help from https://github.com/owncloud/client/blob/cba22aa34b3677406e0499aadd126ce1d94637a2/src/gui/openfilemanager.cpp
void openFileExternal(const std::filesystem::path &filePath) {
    // Make sure the file exists before trying to open it
    if (!std::filesystem::exists(filePath)) {
        return;
    }

#if defined(OS_WINDOWS)
    hex::unused(
        ShellExecuteW(nullptr, L"open", filePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL));
#elif defined(OS_MACOS)
    hex::unused(system(
        hex::format("open {}", wolv::util::toUTF8String(filePath)).c_str()));
#elif defined(OS_LINUX)
    executeCmd({"xdg-open", wolv::util::toUTF8String(filePath)});
#endif
}

void openFolderExternal(const std::filesystem::path &dirPath) {
    // Make sure the folder exists before trying to open it
    if (!std::filesystem::exists(dirPath)) {
        return;
    }

#if defined(OS_WINDOWS)
    auto args = fmt::format(L"\"{}\"", dirPath.c_str());
    ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
#elif defined(OS_MACOS)
    hex::unused(system(
        hex::format("open {}", wolv::util::toUTF8String(dirPath)).c_str()));
#elif defined(OS_LINUX)
    executeCmd({"xdg-open", wolv::util::toUTF8String(dirPath)});
#endif
}

void openFolderWithSelectionExternal(const std::filesystem::path &selectedFilePath) {
    // Make sure the file exists before trying to open it
    if (!std::filesystem::exists(selectedFilePath)) {
        return;
    }

#if defined(OS_WINDOWS)
    auto args = fmt::format(L"/select,\"{}\"", selectedFilePath.c_str());
    ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
#elif defined(OS_MACOS)
    hex::unused(system(
        hex::format(
            R"(osascript -e 'tell application "Finder" to reveal POSIX file "{}"')",
            wolv::util::toUTF8String(selectedFilePath))
            .c_str()));
    system(R"(osascript -e 'tell application "Finder" to activate')");
#elif defined(OS_LINUX)
    // Fallback to only opening the folder for now
    // TODO actually select the file
    executeCmd({"xdg-open", wolv::util::toUTF8String(selectedFilePath.parent_path())});
#endif
}

bool openFileBrowser(DialogMode mode, const std::vector<ItemFilter> &validExtensions, const std::function<void(const char *)> &callback, const std::string &defaultPath, bool multiple) {
    // Turn the content of the ItemFilter objects into something NFD understands
    std::vector<nfdfilteritem_t> validExtensionsNfd;
    validExtensionsNfd.reserve(validExtensions.size());
    for (const auto &extension : validExtensions) {
        validExtensionsNfd.emplace_back(nfdfilteritem_t{extension.name.c_str(), extension.spec.c_str()});
    }

    // Clear errors from previous runs
    NFD::ClearError();

    // Try to initialize NFD
    if (NFD::Init() != NFD_OKAY) {
        // Handle errors if initialization failed
        SPDLOG_ERROR("NFD init returned an error: {}", NFD::GetError());

        return false;
    }

    NFD::UniquePathU8 outPath;
    NFD::UniquePathSet outPaths;
    nfdresult_t result = NFD_ERROR;
    nfdwindowhandle_t nativeWindow{};
    NFD_GetNativeWindowFromGLFWWindow(backend::window, &nativeWindow);

    // Open the correct file dialog based on the mode
    switch (mode) {
        case DialogMode::Open:
            if (multiple)
                result = NFD::OpenDialogMultiple(outPaths, validExtensionsNfd.data(), validExtensionsNfd.size(), defaultPath.empty() ? nullptr : defaultPath.c_str(), nativeWindow);
            else
                result = NFD::OpenDialog(outPath, validExtensionsNfd.data(), validExtensionsNfd.size(), defaultPath.empty() ? nullptr : defaultPath.c_str(), nativeWindow);
            break;
        case DialogMode::Save:
            result = NFD::SaveDialog(outPath, validExtensionsNfd.data(), validExtensionsNfd.size(), defaultPath.empty() ? nullptr : defaultPath.c_str(), nullptr, nativeWindow);
            break;
        case DialogMode::Folder:
            result = NFD::PickFolder(outPath, defaultPath.empty() ? nullptr : defaultPath.c_str(), nativeWindow);
            break;
    }

    if (result == NFD_OKAY) {
        // Handle the path if the dialog was opened in single mode
        if (outPath != nullptr) {
            // Call the provided callback with the path
            callback(outPath.get());
        }

        // Handle multiple paths if the dialog was opened in multiple mode
        if (outPaths != nullptr) {
            nfdpathsetsize_t numPaths = 0;
            if (NFD::PathSet::Count(outPaths, numPaths) == NFD_OKAY) {
                // Loop over all returned paths and call the callback with each of them
                for (size_t i = 0; i < numPaths; i++) {
                    NFD::UniquePathSetPath path;
                    if (NFD::PathSet::GetPath(outPaths, i, path) == NFD_OKAY)
                        callback(path.get());
                }
            }
        }
    } else if (result == NFD_ERROR) {
        // Handle errors that occurred during the file dialog call
        SPDLOG_ERROR("Requested file dialog returned an error: {}", NFD::GetError());
    }

    NFD::Quit();

    return result == NFD_OKAY;
}

}  // namespace imn::fs
