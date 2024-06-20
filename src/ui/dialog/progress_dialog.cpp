
#include "progress_dialog.hpp"

#include <functional>
#include <imgui.h>
#include <string>
#include <tuple>
#include <vector>


void ProgressDialog::cancel() {
    _cancel_callback();
}

void ProgressDialog::show() {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 60), ImGuiCond_Once);

    ImGui::OpenPopup("progress_dialog");
    if (ImGui::BeginPopupModal("progress_dialog")) {
        float progress = float(_current) / _max_v;
        char buf[32];
        sprintf(buf, "%d/%d", _current, _max_v);
        ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

