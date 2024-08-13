
#include "volume_viewer.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include "core/setting.hpp"
#include "include/def.hpp"
#include "ui/style.hpp"
#include "ui/widget/common_widgets.hpp"
#include "util/common.hpp"
#include "util/imgui_util.hpp"

using namespace imn::ui;

VolumeViewer::VolumeViewer() : BaseWindow(), VtkViewer() {
    _id = get_unique_id();
}

VolumeViewer::~VolumeViewer() {
}

void VolumeViewer::set_volume(std::shared_ptr<cv::Mat> vol) {
    std::lock_guard<std::mutex> lock(_mutex);
    _vol = vol;
}

void VolumeViewer::show() {
    std::lock_guard<std::mutex> lock(_mutex);
    // TODO: fix me

        // static auto actor = SetupDemoPipeline();
        // static VtkViewer vtkViewer1;
        // vtkViewer1.addActor(actor);

        // ImGui::SetNextWindowSize(ImVec2(360, 240), ImGuiCond_FirstUseEver);
        // ImGui::Begin("Vtk Viewer 1", nullptr, VtkViewer::NoScrollFlags());
        // vtkViewer1.render();
        // ImGui::End();
        // vtkViewer1.removeActor(actor);


    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Once);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(("image##" + _id).c_str());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::PopStyleVar();
    ImGui::End();
    ImGui::PopStyleVar();
}
