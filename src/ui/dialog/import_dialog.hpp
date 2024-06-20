
#ifndef UI__DIALOG__IMPORT_DIALOG_HPP
#define UI__DIALOG__IMPORT_DIALOG_HPP

#include <imgui.h>
#include <string>
#include <tuple>
#include <vector>
#include "core/io.hpp"


inline std::tuple<bool, Moon::IO::ImportConfig> show_import_dialog(std::vector<std::string> filename) {
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Once);

    static Moon::IO::ImportConfig rv = { 0 };
    rv.common_image = false;
    const char* items[] = { "uint16", "float32" };
    static int item_current = 0;
    bool ok = false;

    ImGui::OpenPopup("ImportFile");
    if (ImGui::BeginPopupModal("ImportFile")) {
        ImGui::Combo("image type", &item_current, items, IM_ARRAYSIZE(items));
        ImGui::InputInt("width", &rv.width);
        ImGui::InputInt("height", &rv.height);
        ImGui::InputInt("offset", &rv.offset);
        ImGui::InputInt("num images", &rv.num_images);
        ImGui::Checkbox("little endian", &rv.little_endian);

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ok = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
    return { ok, rv };
}


#endif
