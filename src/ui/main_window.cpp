
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

// File-Specific Includes
#include "ui/imgui_vtk_demo.h" // Actor generator for this demo
#include "util/imgui_util.hpp"
#include "core/setting.hpp"
#include "core/moon.hpp"


static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void MainWindow::show() {
    auto& g_setting = global_setting();
    Moon moon;
    moon.init();
#ifdef NDEBUG
    // disable warning window in release mode
    vtkObject::GlobalWarningDisplayOff();
#endif
    // Setup pipeline
    auto actor = SetupDemoPipeline();

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return;
    }

    // Use GL 3.2 (All Platforms)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Decide GLSL version
#ifdef __APPLE__
  // GLSL 150
    const char* glsl_version = "#version 150";
#else
  // GLSL 130
    const char* glsl_version = "#version 130";
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui VTKViewer Example", NULL, NULL);
    if (window == NULL) {
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    if (gl3wInit() != 0) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    auto ctx = ImGui::CreateContext();
    ImPlot::CreateContext();
    // ImPlot::SetCurrentContext();
    // ImPlot::SetImGuiContext(ctx);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows'

    auto font_file = g_setting.font_file.c_str();
    auto font_size = g_setting.font_size;
    auto tex = "This is some useful text.你好 μm 你好";
    ImFontConfig config;
    config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(font_file, font_size, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    io.Fonts->AddFontFromFileTTF(font_file, font_size, &config, GetGlyphRangesGreek());
    io.Fonts->Build();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Initialize VtkViewer objects
    VtkViewer vtkViewer1;
    vtkViewer1.addActor(actor);

    VtkViewer vtkViewer2;
    vtkViewer2.getRenderer()->SetBackground(0, 0, 0); // Black background
    vtkViewer2.addActor(actor);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    bool vtk_2_open = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    double f = 0.5;
    GLuint out_texture;
    int out_width;
    int out_height;
    LoadTextureFromFile("asset/image/moon.jpeg", &out_texture, &out_width, &out_height);

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

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        ImGui::ShowStyleEditor();
        ImPlot::ShowDemoWindow();

        {
            ImGui::Begin("image plot");
            if (ImPlot::BeginPlot("##lines_my",ImVec2(0,0))) {
                static ImVec2 bmin(0,0);
                static ImVec2 bmax(1,1);
                static ImVec2 uv0(0,0);
                static ImVec2 uv1(1,1);
                static ImVec4 tint(1,1,1,1);
                ImPlot::SetupAxesLimits(0,1,0,1);
                // ImPlot::PlotImage("my image",ImGui::GetIO().Fonts->TexID, bmin, bmax, uv0, uv1, tint);
                ImPlot::PlotImage("my image", (void*)out_texture, bmin, bmax, uv0, uv1, tint);
                ImPlot::DragLineY(120482,&f,ImVec4(1,0.5f,1,1),1);
                ImPlot::EndPlot();
            }
            ImGui::End();
        }


        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            // ImGui::Text((char*)"This is some useful text.你好 μm 你好");               // Display some text (you can use a format strings too)
            ImGui::Text(tex);               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);
            ImGui::Checkbox("VTK Viewer #2", &vtk_2_open);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) {                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        ImGui::End();

        // 3. Show another simple window.
        if (show_another_window) {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me")) {
                show_another_window = false;
            }
            ImGui::End();
        }

        // 4. Show a simple VtkViewer Instance (Always Open)
        ImGui::SetNextWindowSize(ImVec2(360, 240), ImGuiCond_FirstUseEver);
        ImGui::Begin("Vtk Viewer 1", nullptr, VtkViewer::NoScrollFlags());
        vtkViewer1.render(); // default render size = ImGui::GetContentRegionAvail()
        ImGui::End();

        // 5. Show a more complex VtkViewer Instance (Closable, Widgets in Window)
        ImGui::SetNextWindowSize(ImVec2(720, 480), ImGuiCond_FirstUseEver);
        if (vtk_2_open) {
            ImGui::Begin("Vtk Viewer 2", &vtk_2_open, VtkViewer::NoScrollFlags());

            // Other widgets can be placed in the same window as the VTKViewer
            // However, since the VTKViewer is rendered to size ImGui::GetContentRegionAvail(),
            // it is best to put all widgets first (i.e., render the VTKViewer last).
            // If you want the VTKViewer to be at the top of a window, you can manually calculate
            // and define its size, accounting for the space taken up by other widgets

            auto renderer = vtkViewer2.getRenderer();
            if (ImGui::Button("VTK Background: Black")) {
                renderer->SetBackground(0, 0, 0);
            }
            ImGui::SameLine();
            if (ImGui::Button("VTK Background: Red")) {
                renderer->SetBackground(1, 0, 0);
            }
            ImGui::SameLine();
            if (ImGui::Button("VTK Background: Green")) {
                renderer->SetBackground(0, 1, 0);
            }
            ImGui::SameLine();
            if (ImGui::Button("VTK Background: Blue")) {
                renderer->SetBackground(0, 0, 1);
            }
            static float vtk2BkgAlpha = 0.2f;
            ImGui::SliderFloat("Background Alpha", &vtk2BkgAlpha, 0.0f, 1.0f);
            renderer->SetBackgroundAlpha(vtk2BkgAlpha);

            vtkViewer2.render();

            ImGui::End();
        }

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
