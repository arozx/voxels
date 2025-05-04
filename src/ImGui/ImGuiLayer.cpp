/**
 * @file ImGuiLayer.cpp
 * @brief Implementation of the ImGui integration layer
 */
#include <pch.h>
#include "ImGuiLayer.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace Engine {
    ImGuiLayer::ImGuiLayer(Window* window) : m_Window(window) {}

    /**
     * @brief Initializes the ImGui layer for the application window.
     * 
     * @param window Pointer to the application window used for rendering.
     * 
     * @details This method sets up the ImGui context, configures rendering options,
     * and initializes platform-specific backends for GLFW and OpenGL. It performs
     * the following key tasks:
     * - Creates an ImGui context
     * - Enables keyboard navigation and docking
     * - Configures viewport support based on the operating system
     * - Sets up a dark color style
     * - Initializes GLFW and OpenGL rendering backends
     * 
     * @note Platform-specific behavior:
     * - On Linux, viewports are disabled
     * - On other platforms, viewports are enabled
     * 
     * @warning Requires a valid Window pointer and working OpenGL context
     */
    void ImGuiLayer::Init(Window* window) {
        m_Window = window;

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        
        #ifdef __linux__
            // Disable viewports on Linux
            io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        #else
            // Enable viewports on other platforms
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        #endif

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Tweak WindowRounding/WindowBg so platform windows can look identical to regular ones
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Initialize platform/renderer backends
        GLFWwindow* native_window = static_cast<GLFWwindow*>(window->GetNativeWindow());
        ImGui_ImplGlfw_InitForOpenGL(native_window, true);
        ImGui_ImplOpenGL3_Init("#version 410");

        LOG_TRACE("ImGui Layer initialized");
    }

    /**
     * @brief Shuts down the ImGui rendering context and associated backends.
     *
     * This method performs cleanup operations for the ImGui library, including:
     * - Shutting down the OpenGL 3 backend
     * - Shutting down the GLFW backend
     * - Destroying the ImGui context
     *
     * @note Logs an informational message upon successful shutdown.
     */
    void ImGuiLayer::Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        LOG_INFO("ImGui shutdown");
    }

    void ImGuiLayer::Begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End() {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(m_Window->GetWidth()), 
            static_cast<float>(m_Window->GetHeight()));

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
}