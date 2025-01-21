#pragma once
#include <imgui.h>

namespace ImGuiWidgetFlameGraph {
    struct FlameGraphSettings {
        float targetFPS = 60.0f;          // Target FPS line
        bool showAverage = true;          // Show average line
        bool showTargetFPS = true;        // Show target FPS line
        bool enableZoom = true;           // Enable zoom controls
        float zoomLevel = 1.0f;           // Current zoom level
        float panOffset = 0.0f;           // Horizontal pan offset
    };

    void PlotFlame(const char* label,
        const std::vector<float>& values,
        FlameGraphSettings& settings,
        float height = 100.0f,
        float min_value = 0.0f,
        float max_value = 0.0f);
}