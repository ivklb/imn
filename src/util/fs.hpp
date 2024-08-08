#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace imn::fs {

enum class DialogMode {
    Open,
    Save,
    Folder
};

struct ItemFilter {
    // Human-friendly name
    std::string name;
    // Extensions that constitute this filter
    std::string spec;
};

void setFileBrowserErrorCallback(const std::function<void(const std::string &)> &callback);
bool openFileBrowser(DialogMode mode, const std::vector<ItemFilter> &validExtensions, const std::function<void(const char*)> &callback, const std::string &defaultPath = {}, bool multiple = false);

void openFileExternal(const std::filesystem::path &filePath);
void openFolderExternal(const std::filesystem::path &dirPath);
void openFolderWithSelectionExternal(const std::filesystem::path &selectedFilePath);

bool isPathWritable(const std::filesystem::path &path);

}  // namespace imn::fs