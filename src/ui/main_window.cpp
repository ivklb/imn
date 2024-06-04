
#include "main_window.hpp"

// Standard Library
#include <iostream>

#include "ui/vtk_viewer.hpp"
#include <vtkSmartPointer.h>
#include <vtkActor.h>
// OpenGL Loader
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// ImGui + imgui-vtk
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include <spdlog/spdlog.h>

// File-Specific Includes
#include "include/def.hpp"
#include "ui/imgui_vtk_demo.h" // Actor generator for this demo
#include "util/imgui_util.hpp"
#include "core/setting.hpp"
#include "core/moon.hpp"


static void _glfw_error_callback(int error, const char* description) {
    spdlog::error("Glfw Error {}: {}", error, description);
}

MainWindow::MainWindow() {
    _setup();
}

void MainWindow::show() {
#ifdef NDEBUG
    // disable warning window in release mode
    vtkObject::GlobalWarningDisplayOff();
#endif
    // Setup pipeline
    auto actor = SetupDemoPipeline();
    VtkViewer vtkViewer1;
    vtkViewer1.addActor(actor);

    // Main loop
    while (!glfwWindowShouldClose(_window)) {
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

#ifndef NDEBUG
        ImGui::ShowDemoWindow();
        ImGui::ShowStyleEditor();
        ImPlot::ShowDemoWindow();
#endif

        {
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

        ImGui::SetNextWindowSize(ImVec2(360, 240), ImGuiCond_FirstUseEver);
        ImGui::Begin("Vtk Viewer 1", nullptr, VtkViewer::NoScrollFlags());
        // default render size = ImGui::GetContentRegionAvail()
        vtkViewer1.render();
        ImGui::End();

        ///////////////////////////////////////////////////
        // Render dear imgui into screen
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(_window, &display_w, &display_h);
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
        glfwSwapBuffers(_window);
    }
    _cleanup();
}

void MainWindow::_setup() {
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
    _window = glfwCreateWindow(1280, 720, kAppName.c_str(), NULL, NULL);
    if (_window == NULL) {
        spdlog::error("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1); // Enable vsync

    if (gl3wInit() != 0) {
        spdlog::error("Failed to initialize OpenGL loader!");
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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup Dear ImGui font
    auto font_file = g_setting.font_file.c_str();
    auto font_size = g_setting.font_size;
    ImFontConfig config;
    config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(font_file, font_size, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    io.Fonts->AddFontFromFileTTF(font_file, font_size, &config, GetGlyphRangesGreek());
    io.Fonts->Build();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init(_glsl_version);

    // Our state
    int out_width;
    int out_height;
    LoadTextureFromFile("asset/image/moon.jpeg", &_out_texture, &out_width, &out_height);
}

void MainWindow::_cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(_window);
    glfwTerminate();
}
