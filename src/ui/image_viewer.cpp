
#include "image_viewer.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>


void ImageViewer::set_image(std::shared_ptr<Dtype> image) {
    _image = image;
}

void ImageViewer::show() {
    ImGui::Begin("unicode 中文 μm");
    if (ImPlot::BeginPlot("##lines_my", ImVec2(0, 0))) {
        static ImVec2 bmin(0, 0);
        static ImVec2 bmax(1, 1);
        static ImVec2 uv0(0, 0);
        static ImVec2 uv1(1, 1);
        static ImVec4 tint(1, 1, 1, 1);
        static double f = 0.5;
        ImPlot::SetupAxesLimits(0, 1, 0, 1);
        ImPlot::PlotImage("my image", (void*)_out_texture, bmin, bmax, uv0, uv1, tint);
        ImPlot::DragLineY(120482, &f, ImVec4(1, 0.5f, 1, 1), 1);
        ImPlot::EndPlot();
    }
    ImGui::End();
}

void ImageViewer::hide() {

}

void ImageViewer::_create_toolbar() {

}

