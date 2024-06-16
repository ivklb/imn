
#include "image_viewer.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include "core/setting.hpp"
#include "include/def.hpp"
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

    {
        ImGui::BeginChild("image");
        auto region = ImGui::GetContentRegionAvail();
        _show_image(ImVec2(region.x, region.y - get_input_box_height()));

        static ImGuiSliderFlags flags = ImGuiSliderFlags_None;
        static int slider_i = 50;
        ImGui::SetNextItemWidth(region.x);
        ImGui::SliderInt("##slider", &slider_i, 0, 100, "%d", flags);
        ImGui::EndChild();
    }
    ImGui::PopStyleVar();
    ImGui::End();
}

void ImageViewer::_show_toolbar() {
    int button_size = 60;
    ImVec2 icon_size = ImVec2(button_size, button_size);
    auto region = ImGui::GetContentRegionAvail();
    auto toolbar_width = button_size + ImGui::GetStyle().FramePadding.x * 2;
    auto toolbar_height = region.y;

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_HorizontalScrollbar
        | ImGuiWindowFlags_NoScrollbar
        ;
    ImGui::BeginChild("toolbar", ImVec2(toolbar_width, toolbar_height), false, window_flags);
    if (ImageButton("##button_none", "asset/image/cursor.png", _mouse_mode == kNone, icon_size)) {
        SPDLOG_DEBUG("mouse mode: none");
        _mouse_mode = kNone;
    }
    if (ImageButton("##button_ruler", "asset/image/line.png", _mouse_mode == kRuler, icon_size)) {
        SPDLOG_DEBUG("mouse mode: ruler");
        _mouse_mode = kRuler;
    }
    if (ImageButton("##button_rect", "asset/image/rectangle.png", _mouse_mode == kRect, icon_size)) {
        SPDLOG_DEBUG("mouse mode: rect");
        _mouse_mode = kRect;
    }
    ImGui::EndChild();
}

void ImageViewer::_show_image(ImVec2 region) {
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));

    if (ImPlot::BeginPlot("##image_viewer", region, ImPlotFlags_CanvasOnly)) {
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::Selectable("reset")) {
                // TODO:
                SPDLOG_DEBUG("reset");
            }
            if (ImGui::Selectable("toggle horizontal line", _show_horizontal_line)) {
                _show_horizontal_line = !_show_horizontal_line;
            }
            ImGui::EndPopup();
        }
        if (_last_canvas_size.x != region.x || _last_canvas_size.y != region.y) {
            SPDLOG_DEBUG("region changed: {} {}", region.x, region.y);
            _last_canvas_size = region;
            auto [p1, p2] = _calc_paint_region(_image->cols, _image->rows, region.x, region.y);
            _bounds_min = p1;
            _bounds_max = p2;
            _drag_y = region.y / 2;
            ImPlot::SetupAxesLimits(0, region.x, 0, region.y, ImPlotCond_Always);

            _ruler_points[0] = region.x * 0.25;
            _ruler_points[1] = region.x * 0.75;
            _ruler_points[2] = region.y * 0.5;
            _ruler_points[3] = region.y * 0.5;

            _rect[0] = region.x * 0.25;
            _rect[1] = region.y * 0.25;
            _rect[2] = region.x * 0.75;
            _rect[3] = region.y * 0.75;
        } else {
        }
        // ImPlot::SetupAxes(NULL, NULL,
        //     ImPlotAxisFlags_NoDecorations,
        //     ImPlotAxisFlags_NoDecorations
        // );
        ImPlot::PlotImage("##image", _tex_id, _bounds_min, _bounds_max);
        if (_show_horizontal_line) {
            ImPlot::DragLineY(120482, &_drag_y, ImVec4(1, 1, 1, 1), 1);
        }
        if (_mouse_mode == kRuler) {
            ImPlot::DragPoint(120483, &_ruler_points[0], &_ruler_points[2], kColorYellow, 5);
            ImPlot::DragPoint(120484, &_ruler_points[1], &_ruler_points[3], kColorYellow, 5);
            ImPlot::SetNextLineStyle(kColorYellow);
            ImPlot::PlotLine("##ruler", &_ruler_points[0], &_ruler_points[2], 2, 0, sizeof(double));
        }
        if (_mouse_mode == kRect) {
            ImPlot::DragRect(120485, &_rect[0], &_rect[1], &_rect[2], &_rect[3], kColorYellow);
        }
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
