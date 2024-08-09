
#include "image_viewer.hpp"

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

ImageViewer::ImageViewer() {
    _id = get_unique_id();
}

ImageViewer::~ImageViewer() {
    for (auto& [img_idx, tex_id] : _tex_id_map) {
        try {
            glDeleteTextures(1, (GLuint*)&tex_id);
        } catch (const std::exception& e) {
            SPDLOG_DEBUG("{}", e.what());
        }
    }
    _tex_id_map.clear();
}

void ImageViewer::set_image(std::shared_ptr<cv::Mat> image) {
    _images.clear();
    _images.push_back(image);
    _img_idx = 0;
}

void ImageViewer::set_images(std::vector<std::shared_ptr<cv::Mat>> images) {
    _images = images;
    _img_idx = 0;
}

void ImageViewer::show() {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Once);

    ImGui::Begin(("中文 μm image##" + _id).c_str());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    _show_toolbar();
    ImGui::SameLine();

    {
        ImGui::BeginChild("image");
        auto region = ImGui::GetContentRegionAvail();
        _show_image(ImVec2(region.x, region.y - get_input_box_height()));

        if (_images.size() > 1) {
            static ImGuiSliderFlags flags = ImGuiSliderFlags_None;
            ImGui::SetNextItemWidth(region.x);
            ImGui::SliderInt("##slider", &_img_idx, 0, _images.size() - 1, "%d", flags);
        }
        ImGui::EndChild();
    }
    ImGui::PopStyleVar();
    ImGui::End();
}

void ImageViewer::_show_toolbar() {
    ImVec2 icon_size = get_style().image_button_size;

    auto region = ImGui::GetContentRegionAvail();
    auto toolbar_width = icon_size.x + ImGui::GetStyle().FramePadding.x * 2;
    auto toolbar_height = region.y;

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollbar;
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
            auto [p1, p2] = _calc_paint_region(_images[0]->cols, _images[0]->rows, region.x, region.y);
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
        // if (!_tex_id_map.contains(_img_idx)) {
        //     _tex_id_map[_img_idx] = load_texture_2d(_images[_img_idx].get());
        // } else {
        // TODO: remove me, test only
        try {
            glDeleteTextures(1, (GLuint*)&_tex_id_map[_img_idx]);
        } catch (const std::exception& e) {
            SPDLOG_ERROR("{}", e.what());
        }
        _tex_id_map[_img_idx] = load_texture_2d(_images[_img_idx].get());
        // }
        ImPlot::PlotImage("##image", _tex_id_map[_img_idx], _bounds_min, _bounds_max);
        if (_show_horizontal_line) {
            ImPlot::DragLineY(1, &_drag_y, ImVec4(1, 1, 1, 1), 1);
        }
        if (_mouse_mode == kRuler) {
            ImPlot::DragPoint(2, &_ruler_points[0], &_ruler_points[2], kColorYellow, 5);
            ImPlot::DragPoint(3, &_ruler_points[1], &_ruler_points[3], kColorYellow, 5);
            ImPlot::SetNextLineStyle(kColorYellow);
            ImPlot::PlotLine("##ruler", &_ruler_points[0], &_ruler_points[2], 2, 0, sizeof(double));
        }
        if (_mouse_mode == kRect) {
            ImPlot::DragRect(4, &_rect[0], &_rect[1], &_rect[2], &_rect[3], kColorYellow);
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
    return {p1, p2};
}
