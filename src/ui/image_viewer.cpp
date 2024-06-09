
#include "image_viewer.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include "util/imgui_util.hpp"
#include "ui/widget/common_widgets.hpp"


void ImageViewer::set_image(std::shared_ptr<cv::Mat> image) {
    _image = image;
    _tex_id = load_texture_2d(image.get());
}

void ImageViewer::show() {
    ImGui::Begin("unicode 中文 μm");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    _show_toolbar();
    ImGui::SameLine();
    _show_image();
    ImGui::PopStyleVar();
    ImGui::End();
}

void ImageViewer::_show_toolbar() {
    int button_size = 60;
    auto region = ImGui::GetContentRegionAvail();
    auto toolbar_width = button_size + ImGui::GetStyle().FramePadding.x * 2;
    auto toolbar_height = region.y;

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_HorizontalScrollbar
        | ImGuiWindowFlags_NoScrollbar
        ;
    ImGui::BeginChild("ChildL", ImVec2(toolbar_width, toolbar_height), false, window_flags);
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(button_size, button_size));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(button_size, button_size));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(button_size, button_size));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(button_size, button_size));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(button_size, button_size));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(button_size, button_size));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(button_size, button_size));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(button_size, button_size));
    ImageButton("##button", "asset/image/moon.jpeg", ImVec2(button_size, button_size));
    ImGui::EndChild();
}

void ImageViewer::_show_image() {
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
    static ImVec2 last_region;
    auto region = ImGui::GetContentRegionAvail();
    if (ImPlot::BeginPlot("##lines_my", region, ImPlotFlags_CanvasOnly)) {
        static ImVec2 bmin;
        static ImVec2 bmax;
        static double f = region.y / 2;
        if (last_region.x != region.x || last_region.y != region.y) {
            SPDLOG_DEBUG("region changed: {} {}", region.x, region.y);
            last_region = region;
            auto [p1, p2] = _calc_paint_region(_image->cols, _image->rows, region.x, region.y);
            bmin = p1;
            bmax = p2;
            ImPlot::SetupAxesLimits(0, region.x, 0, region.y, ImPlotCond_Always);
        } else {
        }
        ImPlot::SetupAxes(NULL, NULL,
            ImPlotAxisFlags_NoDecorations,
            ImPlotAxisFlags_NoDecorations
        );
        ImPlot::PlotImage("##image", _tex_id, bmin, bmax);
        ImPlot::DragLineY(120482, &f, ImVec4(1, 1, 1, 1), 1);
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();
}

std::tuple<ImVec2, ImVec2> ImageViewer::_calc_paint_region(double image_width, double image_height, double canvas_width, double canvas_height) {
    SPDLOG_DEBUG("image_width: {}, image_height: {}, canvas_width: {}, canvas_height: {}", image_width, image_height, canvas_width, canvas_height);
    double image_ratio = image_width / image_height;
    double canvas_ratio = canvas_width / canvas_height;
    ImVec2 p1, p2;
    if (image_ratio > canvas_ratio) {
        //       +-----------------------+ (canvas_width,canvas_height)
        //       |        <blank>        |
        //       +-----------------------+
        //       |         image         |
        //       +-----------------------+
        //       |        <blank>        |
        // (0,0) +-----------------------+
        double paint_height = image_height / image_width * canvas_width;
        p1 = ImVec2(0, (canvas_height - paint_height) / 2);
        p2 = ImVec2(canvas_width, (canvas_height + paint_height) / 2);
    } else {
        //       +-------+-------+-------+ (canvas_width,canvas_height)
        //       |       |       |       |
        //       |<blank>| image |<blank>|
        //       |       |       |       |
        // (0,0) +-------+-------+-------+
        double paint_width = image_width / image_height * canvas_height;
        p1 = ImVec2((canvas_width - paint_width) / 2, 0);
        p2 = ImVec2((canvas_width + paint_width) / 2, canvas_height);
    }
    SPDLOG_DEBUG("p1: {}, {} p2: {}, {}", p1.x, p1.y, p2.x, p2.y);
    return { p1, p2 };
}
