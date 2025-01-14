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
    void ImGuiLayer::Init(Window* window) {
        m_Window = window;
        
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        // Enable basic keyboard navigation
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer bindings
        GLFWwindow* native_window = static_cast<GLFWwindow*>(window->GetNativeWindow());
        ImGui_ImplGlfw_InitForOpenGL(native_window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        LOG_INFO("ImGui initialized");
    }

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
    }
}