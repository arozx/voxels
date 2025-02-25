#include "ImGuiOverlay.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>

#include "../Scene/SceneManager.h"
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
     * @brief Renders performance statistics overlay with detailed FPS and frame time information
     * 
     * Displays a window in the top-right corner of the screen showing current performance metrics,
     * including current and average FPS, frame time, 1% low and high FPS values. Provides interactive
     * controls for VSync and flame graph visualization.
     * 
     * @param renderObject The render object context (currently unused in implementation)
     * @param showFPSCounter Flag to control visibility of the FPS counter
     * @param currentFPS Instantaneous frames per second for the current frame
     * @param averageFPS Rolling average of frames per second
     * @param frameTime Duration of the last frame in seconds
     * @param fps1PercentLow 1% low performance frame rate
     * @param fps1PercentHigh 1% high performance frame rate
     * 
     * @note The overlay can be toggled using the F3 key
     * @note Includes interactive VSync toggle and flame graph with configurable settings
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
     * @brief Renders interactive transform controls for a RenderObject
     * 
     * Displays a window with drag controls for modifying an object's position, rotation, and scale.
     * The window is only rendered if transform controls are enabled via m_ShowTransformControls.
     * 
     * @param renderObject Reference to the RenderObject whose transform will be manipulated
     * 
     * @note Controls use ImGui drag float widgets with a sensitivity of 0.1 units per drag
     * @note Position, rotation, and scale are modified directly on the object's transform
     */
    void ImGuiOverlay::RenderTransformControls(Engine::RenderObject& object) {
        auto& transform = object.GetTransform();
        auto& data = transform.GetData();  // Get mutable reference to transform data

        if (ImGui::CollapsingHeader("Transform")) {
            ImGui::DragFloat3("Position", &data.position[0], 0.1f);
            ImGui::DragFloat3("Rotation", &data.rotation[0], 0.1f);
            ImGui::DragFloat3("Scale", &data.scale[0], 0.1f);
        }
    }

    /**
     * @brief Renders a comprehensive profiler window for performance metrics and analysis
     * 
     * This method displays a window that allows users to control and view profiling information.
     * Features include:
     * - Enabling/disabling profiling
     * - Clearing existing profiling data
     * - Configuring number of frames to profile
     * - Displaying detailed performance statistics for each profiled function
     * 
     * @note The window is only rendered if m_ShowProfiler is true
     * @note Profiling data includes average, minimum, maximum execution times, and number of calls
     */
    void ImGuiOverlay::RenderProfiler() const {
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
     * @brief Renders the renderer settings window with back-face culling controls
     * 
     * @details Provides an interactive ImGui window to toggle back-face culling in OpenGL.
     * When enabled, back-face culling is set to cull back faces with counter-clockwise winding order.
     * The current culling state is retrieved and can be modified through a checkbox.
     * 
     * @note The window is only rendered if m_ShowRendererSettings is true
     * 
     * @pre OpenGL context must be initialized
     * @post OpenGL culling state may be modified based on user interaction
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
     * @brief Renders a window displaying recent events with detailed information
     * 
     * @details Displays a debug window showing the last 5 events in the event history.
     * Each event can be expanded to reveal additional details such as:
     * - Event name and time since occurrence
     * - Event priority (High, Normal, Low)
     * - Handled status
     * - Detailed debug information
     * 
     * The events are color-coded based on their recency, with recently occurred events 
     * highlighted in green. If no events have been recorded, a placeholder message is shown.
     * 
     * @note The window is only rendered if m_ShowEventDebugger is true
     * @note Uses ImGui for rendering the event debug interface
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
     * @brief Renders interactive controls for terrain generation and modification
     * 
     * @details Provides a comprehensive ImGui window for adjusting terrain generation parameters.
     * Allows users to:
     * - Modify chunk rendering range
     * - Regenerate terrain with a specific seed
     * - Adjust terrain parameters like base height, height scale, and noise scale
     * - Toggle automatic parameter updates
     * 
     * @note Requires an active scene with a valid terrain system
     * @note Uses static variables to maintain parameter state between renders
     * 
     * @warning Skips rendering if no active scene or terrain system is available
     */
    void ImGuiOverlay::RenderTerrainControls() {
        auto activeScene = Engine::SceneManager::Get().GetActiveScene();
        if (!activeScene) return;

        auto terrainSystem = activeScene->GetTerrainSystem();
        if (!terrainSystem) return;

        if (ImGui::Begin("Terrain Controls")) {
            int chunkRange = terrainSystem->GetChunkRange();
            if (ImGui::SliderInt("Chunk Range", &chunkRange, 0, 5)) {
                terrainSystem->SetChunkRange(chunkRange);
            }

            // Terrain seed control
            static uint32_t seed = 1234;

            if (ImGui::Button("Regenerate Terrain")) {
                terrainSystem->RegenerateTerrain(seed);
            }

            // Terrain parameters
            ImGui::Separator();
            ImGui::Text("Terrain Parameters:");

            static float baseHeight = 32.0f;
            static float heightScale = 32.0f;
            static float noiseScale = 0.05f;
            static bool autoUpdate = false;

            if (ImGui::SliderFloat("Base Height", &baseHeight, 0.0f, 64.0f) && autoUpdate) {
                terrainSystem->SetBaseHeight(baseHeight);
            }
            if (ImGui::SliderFloat("Height Scale", &heightScale, 1.0f, 64.0f) && autoUpdate) {
                terrainSystem->SetHeightScale(heightScale);
            }
            if (ImGui::SliderFloat("Noise Scale", &noiseScale, 0.01f, 0.2f) && autoUpdate) {
                terrainSystem->SetNoiseScale(noiseScale);
            }

            ImGui::Checkbox("Auto Update", &autoUpdate);

            if (!autoUpdate && ImGui::Button("Apply Parameters")) {
                terrainSystem->SetBaseHeight(baseHeight);
                terrainSystem->SetHeightScale(heightScale);
                terrainSystem->SetNoiseScale(noiseScale);
                terrainSystem->RegenerateTerrain(seed);
            }
        }
        ImGui::End();
    }
    }  // namespace Engine