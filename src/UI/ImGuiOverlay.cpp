#include "ImGuiOverlay.h"
#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Debug/Profiler.h"

namespace Engine {
    ImGuiOverlay::ImGuiOverlay(Window* window) 
        : m_Window(window), m_Renderer(&Renderer::Get()) {}

    void ImGuiOverlay::OnRender(RenderObject& renderObject, bool showFPSCounter,
        float currentFPS, float averageFPS, float frameTime,
        float fps1PercentLow, float fps1PercentHigh) 
    {
        if (showFPSCounter) {
            ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Camera Type: %s", m_Renderer->GetCameraType() == Renderer::CameraType::Orthographic ? "Orthographic" : "Perspective");
            ImGui::Separator();
            ImGui::Text("Current FPS: %.1f", currentFPS);
            ImGui::Text("Average FPS: %.1f", averageFPS);
            ImGui::Text("Frame Time: %.2f ms", frameTime * 1000.0f);
            ImGui::Text("1%% Low: %.1f", fps1PercentLow);
            ImGui::Text("1%% High: %.1f", fps1PercentHigh);
            ImGui::Separator();
            
            bool vsync = m_Window->IsVSync();
            if (ImGui::Checkbox("VSync", &vsync)) {
                m_Window->SetVSync(vsync);
            }
            
            ImGui::Text("Press F3 to toggle FPS counter");
            ImGui::End();
        }
    }

    void ImGuiOverlay::RenderTransformControls(RenderObject& renderObject) {
        if (ImGui::Begin("Transform Controls")) {
            auto& transform = renderObject.GetTransform();
            
            ImGui::DragFloat3("Position", &transform.position[0], 0.1f);
            ImGui::DragFloat3("Rotation", &transform.rotation[0], 0.1f);
            ImGui::DragFloat3("Scale", &transform.scale[0], 0.1f);
        }
        ImGui::End();
    }

    void ImGuiOverlay::RenderProfiler() {
        ImGui::Begin("Profiler", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        bool enabled = Profiler::Get().IsEnabled();
        if (ImGui::Checkbox("Enable Profiling", &enabled)) {
            Profiler::Get().SetEnabled(enabled);
        }

        if (ImGui::Button("Clear Profiling Data")) {
            Profiler::Get().ClearProfiles();
        }

        ImGui::Separator();

        for (const auto& [name, timings] : Profiler::Get().GetProfiles()) {
            if (timings.empty()) continue;
            
            float total = 0.0f;
            for (float time : timings) {
                total += time;
            }
            
            float avg = total / static_cast<float>(timings.size());
            float min = *std::min_element(timings.begin(), timings.end());
            float max = *std::max_element(timings.begin(), timings.end());
            
            ImGui::Text("%s:", name.c_str());
            ImGui::Text("  Avg: %.3f ms", avg);
            ImGui::Text("  Min: %.3f ms", min);
            ImGui::Text("  Max: %.3f ms", max);
            ImGui::Text("  Calls: %zu", timings.size());
            ImGui::Separator();
        }
        ImGui::End();
    }

    void ImGuiOverlay::RenderRendererSettings() {
        ImGui::Begin("Renderer");
        bool cullingEnabled = glIsEnabled(GL_CULL_FACE);
        if (ImGui::Checkbox("Enable Back-face Culling", &cullingEnabled)) {
            if (cullingEnabled) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                glFrontFace(GL_CCW);
            } else {
                glDisable(GL_CULL_FACE);
            }
        }
        ImGui::End();
    }
}