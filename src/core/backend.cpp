

#include "backend.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

#include <functional>

#include "core/cache.hpp"
#include "include/def.hpp"

namespace imn::backend {

namespace impl {
const char* glsl_version = "#version 130";
static void _glfw_error_callback(int error, const char* description) {
    SPDLOG_ERROR("Glfw Error {}: {}", error, description);
}
}  // namespace impl

void setup() {
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

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(impl::glsl_version);
}

void cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void show(std::function<void()> frame_callback) {
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

ImTextureID load_texture_2d(const std::string& img, bool nearest_sample) {
    if (Cache::has(img)) {
        return Cache::get<ImTextureID>(img);
    }

    auto mat = cv::imread(img, cv::IMREAD_UNCHANGED);
    auto rv = load_texture_2d(&mat, nearest_sample);
    Cache::add(img, rv);
    return rv;
}

ImTextureID load_texture_2d(const cv::Mat* img, bool nearest_sample) {
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    if (nearest_sample) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    // https://stackoverflow.com/questions/16809833/opencv-image-loading-for-opengl-texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, (img->step & 3) ? 1 : 4);

    if (img->type() == CV_8UC1) {
        // TODO: review me
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, img->cols, img->rows, 0, GL_RED, GL_UNSIGNED_BYTE, img->data);
    } else if (img->type() == CV_8UC3) {
        // jpeg
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->cols, img->rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img->data);
    } else if (img->type() == CV_8UC4) {
        // png
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->cols, img->rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, img->data);
    } else if (img->type() == CV_16UC1) {
        // TODO: review me
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, img->cols, img->rows, 0, GL_RED, GL_UNSIGNED_SHORT, img->data);
    } else if (img->type() == CV_32FC1) {
        // TODO: review me
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, img->cols, img->rows, 0, GL_RED, GL_FLOAT, img->data);
    } else {
        SPDLOG_ERROR("unsupported image type {}", img->type());
    }
    return (ImTextureID)image_texture;
}

}  // namespace imn::backend
