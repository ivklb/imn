
#include "image_viewer.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include "util/imgui_util.hpp"
#include "ui/widget/common_widgets.hpp"


void ImageViewer::set_image(std::shared_ptr<cv::Mat> image) {
    _image = image;
    _tex_id = (void*)load_texture_2d(image.get());
}

void ImageViewer::show() {
    ImGui::Begin("unicode 中文 μm");
    _show_toolbar();
    ImGui::SameLine();
    _show_image();
    ImGui::End();
}

void ImageViewer::_show_toolbar() {
    auto region = ImGui::GetContentRegionAvail();
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_HorizontalScrollbar
        | ImGuiWindowFlags_NoScrollbar
        ;
    // ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), false, window_flags);
    ImGui::BeginChild("ChildL", ImVec2(70, 260), false, window_flags);
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(60, 60));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(60, 60));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(60, 60));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(60, 60));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(60, 60));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(60, 60));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(60, 60));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(60, 60));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(60, 60));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(60, 60));
    ImGui::EndChild();
}

void ImageViewer::_show_image() {
    if (ImPlot::BeginPlot("##lines_my", ImVec2(0, 0), ImPlotFlags_CanvasOnly)) {
        static ImVec2 bmin(0, 0);
        static ImVec2 bmax(1, 1);
        static ImVec2 uv0(0, 0);
        static ImVec2 uv1(1, 1);
        static ImVec4 tint(1, 1, 1, 1);
        static double f = 0.5;
        ImPlot::SetupAxesLimits(0, 1, 0, 1);
        ImPlot::SetupAxes("", "", ImPlotAxisFlags_NoDecorations);
        ImPlot::PlotImage("my image", (void*)_tex_id, bmin, bmax, uv0, uv1, tint);
        ImPlot::DragLineY(120482, &f, ImVec4(1, 0.5f, 1, 1), 1);
        ImPlot::EndPlot();
    }
}
