#include "ImGuiOverlay.h"
#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Debug/Profiler.h"
#include "ImGuiFlameGraph.h"

namespace Engine {
    /**
     * @brief Constructor for ImGui overlay
     * @param window Pointer to the window instance
     */
    ImGuiOverlay::ImGuiOverlay(Window* window) 
        : m_Window(window), m_Renderer(&Renderer::Get()) {
        // Initialize flame graph settings with defaults
        m_FlameGraphSettings = ImGuiWidgetFlameGraph::FlameGraphSettings();
    }

    /**
     * @brief Render performance statistics overlay
     * @param renderObject The object being rendered
     * @param showFPSCounter Whether to show FPS counter
     * @param currentFPS Current frames per second
     * @param averageFPS Average frames per second
     * @param frameTime Time taken for last frame
     * @param fps1PercentLow 1% low FPS value
     * @param fps1PercentHigh 1% high FPS value
     */
    void ImGuiOverlay::OnRender(RenderObject& renderObject, bool showFPSCounter,
        float currentFPS, float averageFPS, float frameTime,
        float fps1PercentLow, float fps1PercentHigh) 
    {
        if (!m_ShowFPSCounter) return;  // Early return if FPS counter is disabled
        m_FPSCounter.Update(frameTime);

        if (showFPSCounter) {
            // Calculate max width (minimum of 25% screen width or 300px)
            float maxWidth = std::min(ImGui::GetMainViewport()->Size.x * 0.25f, 300.0f);
            
            ImGui::SetNextWindowSizeConstraints(
                ImVec2(0, 0),           // Min size
                ImVec2(maxWidth, FLT_MAX)  // Max size
            );
            
            ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            
            // Set initial window position to top-right if not already set
            if (ImGui::GetWindowPos().x == 0 && ImGui::GetWindowPos().y == 0) {
                ImGui::SetWindowPos(ImVec2(
                    ImGui::GetMainViewport()->Size.x - maxWidth,
                    0
                ));
            }

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

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            // Add flame graph with minimum size requirements
            const auto& frameHistory = m_FPSCounter.GetFrameTimeHistory();
            if (!frameHistory.empty()) {
                ImGui::Separator();
                ImGui::Text("Frame Time Analysis");
                
                // Create a more compact horizontal layout
                float buttonWidth = 70.0f; // Smaller reset button
                float settingsWidth = ImGui::GetWindowWidth() - buttonWidth - 20.0f;
                
                ImGui::BeginGroup();
                ImGui::PushItemWidth(settingsWidth);
                if (ImGui::TreeNode("Graph Settings")) {
                    ImGui::Indent(5.0f); // Reduced indentation
                    ImGui::SliderFloat("##FPS", &m_FlameGraphSettings.targetFPS, 30.0f, 240.0f, "%.0f FPS"); // Compact slider
                    ImGui::SameLine(); ImGui::Text("Target");
                    ImGui::Checkbox("Avg", &m_FlameGraphSettings.showAverage);
                    ImGui::SameLine();
                    ImGui::Checkbox("Target", &m_FlameGraphSettings.showTargetFPS);
                    ImGui::SameLine();
                    ImGui::Checkbox("Zoom", &m_FlameGraphSettings.enableZoom);
                    ImGui::Unindent(5.0f);
                    ImGui::TreePop();
                }
                ImGui::PopItemWidth();
                
                ImGui::SameLine(ImGui::GetWindowWidth() - buttonWidth - 10.0f);
                if (ImGui::Button("Reset", ImVec2(buttonWidth, 0))) {
                    m_FlameGraphSettings.zoomLevel = 1.0f;
                    m_FlameGraphSettings.panOffset = 0.0f;
                }
                ImGui::EndGroup();
                
                ImGui::Spacing();
                // Reserve minimum space for the graph
                ImGuiWidgetFlameGraph::PlotFlame(
                    "ms", // Shorter label
                    frameHistory,
                    m_FlameGraphSettings,
                    100.0f // Fixed height
                );
            }
            ImGui::End();
        }
    }

    /**
     * @brief Render transform controls for an object
     * @param renderObject Object whose transform is being controlled
     */
    void ImGuiOverlay::RenderTransformControls(RenderObject& renderObject) {
        if (!m_ShowTransformControls) return;
        if (ImGui::Begin("Transform Controls")) {
            auto& transform = renderObject.GetTransform();
            
            ImGui::DragFloat3("Position", &transform.position[0], 0.1f);
            ImGui::DragFloat3("Rotation", &transform.rotation[0], 0.1f);
            ImGui::DragFloat3("Scale", &transform.scale[0], 0.1f);
        }
        ImGui::End();
    }

    /**
     * @brief Render profiler window showing performance metrics
     */
    void ImGuiOverlay::RenderProfiler() {
        if (!m_ShowProfiler) return;
        ImGui::Begin("Profiler", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        bool enabled = Profiler::Get().IsEnabled();
        if (ImGui::Checkbox("Enable Profiling", &enabled)) {
            Profiler::Get().SetEnabled(enabled);
        }

        if (ImGui::Button("Clear Profiling Data")) {
            Profiler::Get().ClearProfiles();
        }
        
        // Add frame profiling input control
        static int framesToProfile = 60;
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("Frames to Profile", &framesToProfile);
        framesToProfile = std::max(1, std::min(framesToProfile, 1000)); // Clamp between 1-1000
        
        if (!Profiler::Get().IsProfilingFrames() && ImGui::Button("Profile Frames")) {
            Profiler::Get().BeginSession("Frame Profile");
            Profiler::Get().ProfileFrames(framesToProfile);
        }
        
        // Show profiling progress if active
        if (Profiler::Get().IsProfilingFrames()) {
            ImGui::SameLine();
            ImGui::Text("Profiling Frame: %d/%d", 
                Profiler::Get().GetCurrentProfiledFrame(),
                framesToProfile);
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

    /**
     * @brief Render renderer settings window
     * @details Controls settings like back-face culling
     */
    void ImGuiOverlay::RenderRendererSettings() {
        if (!m_ShowRendererSettings) return;
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

    /**
     * @brief Render event debugger window
     * @details Shows recent events and their details
     */
    void ImGuiOverlay::RenderEventDebugger() {
        if (!m_ShowEventDebugger) return;
        if (ImGui::Begin("Recent Events", &m_ShowEventDebugger)) {
            ImGui::Text("Last 5 Events:");
            ImGui::Separator();

            const auto& history = EventDebugger::Get().GetEventHistory();
            for (const auto& eventInfo : history) {
                ImGui::PushStyleColor(ImGuiCol_Header, 
                    eventInfo.timeAgo < 1.0f ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                
                if (ImGui::CollapsingHeader(
                    (eventInfo.name + " [" + std::to_string(eventInfo.timeAgo).substr(0, 4) + "s ago]").c_str())) 
                {
                    ImGui::Indent();
                    ImGui::Text("Priority: %s", 
                        eventInfo.priority == EventPriority::High ? "High" :
                        eventInfo.priority == EventPriority::Normal ? "Normal" : "Low");
                    ImGui::Text("Handled: %s", eventInfo.handled ? "Yes" : "No");
                    ImGui::TextWrapped("Details: %s", eventInfo.debugInfo.c_str());
                    ImGui::Unindent();
                }
                ImGui::PopStyleColor();
            }

            if (history.empty()) {
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No events recorded yet");
            }
        }
        ImGui::End();
    }

    /**
     * @brief Render terrain generation controls
     * @param terrainSystem Reference to the terrain system
     * @details Controls terrain parameters like chunk range, seed, and height settings
     */
    void ImGuiOverlay::RenderTerrainControls(TerrainSystem& terrainSystem) {
        if (!m_ShowTerrainControls) return;
        if (ImGui::Begin("Terrain Controls")) {
            // Chunk range control
            int chunkRange = terrainSystem.GetChunkRange();
            if (ImGui::SliderInt("Chunk Range", &chunkRange, 0, 5)) {
                terrainSystem.SetChunkRange(chunkRange);
            }

            // Terrain seed control
            static uint32_t seed = 1234;
            if (ImGui::InputScalar("Terrain Seed", ImGuiDataType_U32, &seed)) {
                // No need for validation since uint32_t can't be negative
            }
            if (ImGui::Button("Regenerate Terrain")) {
                terrainSystem.RegenerateTerrain(seed);
            }

            // Terrain parameters
            ImGui::Separator();
            ImGui::Text("Terrain Parameters:");
            
            static float baseHeight = 32.0f;
            static float heightScale = 32.0f;
            static float noiseScale = 0.05f;
            static bool autoUpdate = false;

            if (ImGui::SliderFloat("Base Height", &baseHeight, 0.0f, 64.0f) && autoUpdate) {
                terrainSystem.SetBaseHeight(baseHeight);
            }
            if (ImGui::SliderFloat("Height Scale", &heightScale, 1.0f, 64.0f) && autoUpdate) {
                terrainSystem.SetHeightScale(heightScale);
            }
            if (ImGui::SliderFloat("Noise Scale", &noiseScale, 0.01f, 0.2f) && autoUpdate) {
                terrainSystem.SetNoiseScale(noiseScale);
            }

            ImGui::Checkbox("Auto Update", &autoUpdate);

            if (!autoUpdate && ImGui::Button("Apply Parameters")) {
                terrainSystem.SetBaseHeight(baseHeight);
                terrainSystem.SetHeightScale(heightScale);
                terrainSystem.SetNoiseScale(noiseScale);
                terrainSystem.RegenerateTerrain(seed);  // Fixed: Added seed parameter
            }
        }
        ImGui::End();
    }
}