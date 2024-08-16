
#include "main_window.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>

#include <algorithm>
#include <functional>
#include <iostream>

#include "core/app.hpp"
#include "core/backend.hpp"
#include "core/i18n.hpp"
#include "core/lambda.hpp"
#include "core/setting.hpp"
#include "ext/imgui_notify/ImGuiNotify.hpp"
#include "include/def.hpp"
#include "ui/imgui_helper.hpp"
#include "ui/imgui_vtk_demo.h"  // Actor generator for this demo
#include "ui/widgets/image_widget.hpp"
#include "ui/widgets/vtk_viewer.hpp"

using namespace imn::ui;

MainWindow::MainWindow()
    : _show_imgui_demo(false),
      _show_implot_demo(false),
      _show_style_editor(false),
      _show_help(false) {
    _setup();
}

MainWindow::~MainWindow() {
    _cleanup();
}

void MainWindow::show() {
    backend::show(std::bind(&MainWindow::_on_frame, this));
}

void MainWindow::_setup() {
    auto& g_setting = setting::global_setting();
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    auto ctx = ImGui::CreateContext();
    ImPlot::CreateContext();

    backend::setup();

    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup Dear ImGui font
    auto font_file = g_setting.font_file.c_str();
    auto font_size = get_style().font_size;
    ImFontConfig config;
    config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(font_file, font_size, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    io.Fonts->AddFontFromFileTTF(font_file, font_size, &config, GetGlyphRangesGreek());

    // merge in icons from Font Awesome
    float iconFontSize = font_size * 2.0f / 3.0f;  // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = iconFontSize;
    io.Fonts->AddFontFromFileTTF("asset/font/fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges);
    io.Fonts->Build();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Our state
#ifdef NDEBUG
    // disable warning window in release mode
    vtkObject::GlobalWarningDisplayOff();
#endif
    lambda::store("ADD_WINDOW", [this](std::shared_ptr<BaseWindow> w) {
        std::lock_guard<std::mutex> lock(_mutex_win);
        _windows.push_back(w);
    });
    _node_widget.setup();
}

void MainWindow::_on_frame() {
    _create_dock_space_and_menubar();
    _show_dialog();

    ImGui::Begin("Node Editor");
    _node_widget.show();
    ImGui::End();
    _node_widget.process();

    {
        std::lock_guard<std::mutex> lock(_mutex_win);
        for (auto& viewer : _windows) {
            viewer->show();
        }
    }
}

void MainWindow::_create_dock_space_and_menubar() {
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::Begin("MainDockSpace", NULL, window_flags);

    ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu(I18N_STR("file"))) {
            if (ImGui::MenuItem("Open Image(s)", "Ctrl+O")) {
                _load_as_volume = false;
            }
            if (ImGui::MenuItem("Open Volume")) {
                _load_as_volume = true;
            }
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
                SPDLOG_INFO("Exit");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(I18N_STR("view"))) {
            if (ImGui::BeginMenu(I18N_STR("lang"))) {
                auto& settings = setting::global_setting();
                if (ImGui::MenuItem("en", NULL, settings.lang == "en")) {
                    settings.lang = "en";
                }
                if (ImGui::MenuItem("zh", NULL, settings.lang == "zh")) {
                    settings.lang = "zh";
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(I18N_STR("window"))) {
                if (ImGui::MenuItem(I18N_STR("imgui demo"))) {
                    _show_imgui_demo = true;
                }
                if (ImGui::MenuItem(I18N_STR("implot demo"))) {
                    _show_implot_demo = true;
                }
                if (ImGui::MenuItem(I18N_STR("style editor"))) {
                    _show_style_editor = true;
                }
                if (ImGui::MenuItem(I18N_STR("help"))) {
                    _show_help = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

void MainWindow::_show_dialog() {
    ImGui::RenderNotifications();
    if (_show_imgui_demo) {
        ImGui::ShowDemoWindow(&_show_imgui_demo);
    }
    if (_show_implot_demo) {
        ImPlot::ShowDemoWindow(&_show_implot_demo);
    }
    if (_show_style_editor) {
        ImGui::Begin("Style Editor", &_show_style_editor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (_show_help) {
        ImGui::Begin("help", &_show_help);
        ImGui::TextUnformatted("Right click -- add node");
        ImGui::TextUnformatted("Del -- delete selected node or link");
        ImGui::TextUnformatted("Alt + mouse -- drag canvas");
        ImGui::End();
    }
}

void MainWindow::_cleanup() {
    backend::cleanup();
    ImGui::DestroyContext();
    setting::dump_setting();
}
