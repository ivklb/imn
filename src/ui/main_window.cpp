
#include "main_window.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>

#include <algorithm>
#include <iostream>

#include "core/app.hpp"
#include "core/lambda.hpp"
#include "core/setting.hpp"
#include "include/def.hpp"
#include "ui/dialog/ImGuiFileDialog.h"
#include "ui/dialog/import_dialog.hpp"
#include "ui/image_viewer.hpp"
#include "ui/imgui_vtk_demo.h"  // Actor generator for this demo
#include "ui/style.hpp"
#include "ui/vtk_viewer.hpp"
#include "ui/widget/common_widgets.hpp"
#include "ui/widget/imgui_notify.h"
#include "util/imgui_util.hpp"

using namespace imn::ui;

static void _glfw_error_callback(int error, const char* description) {
    SPDLOG_ERROR("Glfw Error {}: {}", error, description);
}

MainWindow::MainWindow() {
    _setup();
}

void MainWindow::show() {
    auto window = App::app()->main_window_handle();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        _on_frame();

        ///////////////////////////////////////////////////
        // Render dear imgui into screen
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        glfwSwapBuffers(window);
    }
    _cleanup();
}

void MainWindow::_setup() {
#ifdef NDEBUG
    // disable warning window in release mode
    vtkObject::GlobalWarningDisplayOff();
#endif
    lambda::store("ADD_WINDOW", [this](std::shared_ptr<BaseWindow> w) {
        _windows.push_back(w);
    });
    _setup_gl();
    _setup_imgui();
}

void MainWindow::_setup_gl() {
    // Setup window
    glfwSetErrorCallback(_glfw_error_callback);
    if (!glfwInit()) {
        return;
    }

    // Use GL 3.2 (All Platforms)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create window with graphics context
    auto _window = glfwCreateWindow(1280, 720, kAppName.c_str(), NULL, NULL);
    if (_window == NULL) {
        SPDLOG_ERROR("Failed to create GLFW window");
        return;
    }
    App::app()->set_main_window_handle(_window);
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);  // Enable vsync

    if (gl3wInit() != 0) {
        SPDLOG_ERROR("Failed to initialize OpenGL loader!");
        return;
    }
}

void MainWindow::_setup_imgui() {
    auto& g_setting = global_setting();
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    auto ctx = ImGui::CreateContext();
    ImPlot::CreateContext();
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
    ImGui::MergeIconsWithLatestFont(font_size - 6, false);
    io.Fonts->Build();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(0, 0);
    style.FramePadding = ImVec2(4, 4);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(App::app()->main_window_handle(), true);
    ImGui_ImplOpenGL3_Init(_glsl_version);

    // Our state
    int out_width;
    int out_height;
    auto img = cv::imread("asset/image/moon.jpeg", cv::IMREAD_UNCHANGED);
    _image_viewer.set_image(std::make_shared<cv::Mat>(std::move(img)));

    _node_window.setup();
}

void MainWindow::_on_frame() {
    _create_dock_space_and_menubar();
    _show_dialog();

#ifndef NDEBUG
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();
#endif

    {
        // Setup pipeline
        static auto actor = SetupDemoPipeline();
        static VtkViewer vtkViewer1;
        vtkViewer1.addActor(actor);

        ImGui::SetNextWindowSize(ImVec2(360, 240), ImGuiCond_FirstUseEver);
        ImGui::Begin("Vtk Viewer 1", nullptr, VtkViewer::NoScrollFlags());
        vtkViewer1.render();
        ImGui::End();
        vtkViewer1.removeActor(actor);
    }
    _image_viewer.show();
    _node_window.show();

    for (auto& viewer : _windows) {
        viewer->show();
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
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Image(s)", "Ctrl+O")) {
                _load_as_volume = false;
                IGFD::FileDialogConfig config;
                config.path = ".";
                config.flags = ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".*", config);
            }
            if (ImGui::MenuItem("Open Volume")) {
                _load_as_volume = true;
                IGFD::FileDialogConfig config;
                config.path = ".";
                // config.countSelectionMax = 0;
                config.flags = ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".*", config);
            }
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
                SPDLOG_INFO("Exit");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void MainWindow::_show_dialog() {
    // file dialog
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(0.5f, 1.0f, 0.9f, 0.9f));
    if (ImGuiFileDialog::Instance()->Display(
            "ChooseFileDlgKey",
            ImGuiWindowFlags_NoCollapse,
            ImVec2(650, 300))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            auto rv = ImGuiFileDialog::Instance()->GetSelection();
            _files_to_open.clear();
            for (auto const& [filename, fullpath] : rv) {
                _files_to_open.push_back(fullpath);
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // import dialog
    if (_files_to_open.size() > 0) {
        auto filename = _files_to_open[0];
        std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
        bool is_common_image_file =
            filename.ends_with(".jpg") ||
            filename.ends_with(".png") ||
            filename.ends_with(".bmp");
        imn::io::ImportConfig config = {0};
        if (is_common_image_file) {
            config.common_image = true;
        } else {
            auto [ok, conf] = show_import_dialog(_files_to_open);
            if (ok) {
                config = conf;
                // TODO: handle cancelled action
                // https://github.com/bshoshany/thread-pool/blob/master/include/BS_thread_pool_utils.hpp
                // https://github.com/bshoshany/thread-pool/blob/master/include/BS_thread_pool.hpp
                // https://github.com/bshoshany/thread-pool
                // https://github.com/progschj/ThreadPool
            }
        }

        // TODO: use progress bar
        if (_load_as_volume) {
            // TODO: implement me
            _files_to_open.clear();
        } else {
            // TODO: run in thread, add cancel logic
            auto image_stack = imn::io::load_image_stack(_files_to_open, config);
            auto image_viewer = std::make_shared<ImageViewer>();
            image_viewer->set_images(image_stack);
            _windows.push_back(image_viewer);
            ImGui::InsertNotification({ImGuiToastType_Info, 3000, "! %s", "We can also format here:)"});
            _files_to_open.clear();
        }
    } else {
    }

    // progress dialog

    // show notification
    // Render toasts on top of everything, at the end of your code!
    // You should push style vars here
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
    ImGui::RenderNotifications();
    ImGui::PopStyleVar(1);  // Don't forget to Pop()
    ImGui::PopStyleColor(1);
}

void MainWindow::_cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(App::app()->main_window_handle());
    glfwTerminate();
}
