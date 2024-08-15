
#ifndef CORE__BACKEND_GL_HPP
#define CORE__BACKEND_GL_HPP

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

#include <functional>

#include "include/def.hpp"

namespace imn::backend {

inline GLFWwindow* window;

namespace impl {
inline const char* glsl_version = "#version 130";
inline static void _glfw_error_callback(int error, const char* description) {
    SPDLOG_ERROR("Glfw Error {}: {}", error, description);
}
}  // namespace impl

inline void setup() {
    // Setup window
    glfwSetErrorCallback(impl::_glfw_error_callback);
    if (!glfwInit()) {
        return;
    }

    // Use GL 3.2 (All Platforms)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create window with graphics context
    window = glfwCreateWindow(1280, 720, kAppName.c_str(), NULL, NULL);
    if (window == NULL) {
        SPDLOG_ERROR("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    if (gl3wInit() != 0) {
        SPDLOG_ERROR("Failed to initialize OpenGL loader!");
        return;
    }
}

inline void setup_imgui_gl() {
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(impl::glsl_version);
}

inline void cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

inline void show(std::function<void()> frame_callback) {
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

        frame_callback();

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
}

}  // namespace imn::backend

#endif