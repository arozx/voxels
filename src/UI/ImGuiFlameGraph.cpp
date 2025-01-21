#include <pch.h>
#include "ImGuiFlameGraph.h"

namespace ImGuiWidgetFlameGraph {
    template<typename T>
    T Clamp(T value, T min, T max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    void PlotFlame(const char* label, const std::vector<float>& values, FlameGraphSettings& settings, float height, float min_value, float max_value) {
        if (values.empty())
            return;

        ImGui::PushID(label);
        
        // Calculate bounds if not provided
        if (max_value == 0.0f) {
            max_value = *std::max_element(values.begin(), values.end());
        }
        if (min_value == max_value) max_value += 1.0f;

        // Get drawing canvas with fixed size
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        ImVec2 canvas_size(300.0f, height); // Fixed width of 300px
        
        // Ensure minimum sizes
        canvas_size.y = std::max(canvas_size.y, 1.0f);

        // Center the graph horizontally in available space
        float available_width = ImGui::GetContentRegionAvail().x;
        if (available_width > canvas_size.x) {
            canvas_pos.x += (available_width - canvas_size.x) * 0.5f;
        }

        // Create invisible button with validated size
        if (ImGui::InvisibleButton("canvas", canvas_size)) {
            // Handle click if needed
        }
        
        // Only draw if visible and has valid size
        if (ImGui::IsItemVisible() && canvas_size.x > 0 && canvas_size.y > 0) {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            
            // Handle zoom and pan input
            if (settings.enableZoom && ImGui::IsItemHovered()) {
                if (ImGui::GetIO().MouseWheel != 0) {
                    settings.zoomLevel = Clamp(
                        settings.zoomLevel + ImGui::GetIO().MouseWheel * 0.1f, 
                        0.1f, 
                        5.0f
                    );
                }
                if (ImGui::IsMouseDragging(0)) {
                    settings.panOffset += ImGui::GetIO().MouseDelta.x;
                }
            }

            // Apply zoom and pan transformations
            const float adjusted_width = canvas_size.x * settings.zoomLevel;
            const float width_per_item = adjusted_width / static_cast<float>(values.size());
            const float start_x = canvas_pos.x + settings.panOffset;

            // Calculate statistics
            float avg = 0.0f;
            for (float v : values) avg += v;
            avg /= values.size();

            // Draw background
            draw_list->AddRectFilled(canvas_pos, 
                ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
                IM_COL32(30, 30, 30, 255));

            // Draw target FPS line
            if (settings.showTargetFPS) {
                float target_y = canvas_pos.y + (1.0f - (1000.0f/settings.targetFPS - min_value) / (max_value - min_value)) * canvas_size.y;
                draw_list->AddLine(
                    ImVec2(canvas_pos.x, target_y),
                    ImVec2(canvas_pos.x + canvas_size.x, target_y),
                    IM_COL32(255, 255, 0, 128),
                    1.0f
                );
            }

            // Draw bars with gradient color
            for (size_t i = 0; i < values.size(); i++) {
                float normalized_value = (values[i] - min_value) / (max_value - min_value);
                float bar_height = normalized_value * canvas_size.y;
                
                ImVec2 bar_pos = ImVec2(
                    start_x + (i * width_per_item),
                    canvas_pos.y + (canvas_size.y - bar_height)
                );
                ImVec2 bar_end = ImVec2(
                    bar_pos.x + width_per_item - 2, // Reduce bar width by adding gap
                    canvas_pos.y + canvas_size.y
                );

                // Enhanced color gradient (green->yellow->red)
                ImU32 color;
                float severity = values[i] / (1000.0f/settings.targetFPS);
                if (severity <= 1.0f)
                    color = IM_COL32(static_cast<int>(severity * 255), 255, 0, 255);
                else if (severity <= 2.0f)
                    color = IM_COL32(255, static_cast<int>((2.0f - severity) * 255), 0, 255);
                else
                    color = IM_COL32(255, 0, 0, 255);

                draw_list->AddRectFilled(bar_pos, bar_end, color);

                // Show tooltip on hover
                if (ImGui::IsMouseHoveringRect(bar_pos, bar_end)) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Frame %zu: %.2f ms %s", i, values[i], 
                        values[i] > (1000.0f/settings.targetFPS) ? "(Frame drop)" : "");
                    ImGui::EndTooltip();
                }
            }

            // Draw average line
            if (settings.showAverage) {
                float avg_y = canvas_pos.y + (1.0f - (avg - min_value) / (max_value - min_value)) * canvas_size.y;
                draw_list->AddLine(
                    ImVec2(canvas_pos.x, avg_y),
                    ImVec2(canvas_pos.x + canvas_size.x, avg_y),
                    IM_COL32(255, 255, 255, 128),
                    1.0f
                );
            }

            // Draw statistics overlay with background - moved to top-left
            char stats_buf[64];
            snprintf(stats_buf, sizeof(stats_buf), "%.1fms\n%.1fx", avg, settings.zoomLevel);
            
            ImVec2 text_size = ImGui::CalcTextSize(stats_buf);
            ImVec2 padding(3.0f, 2.0f);
            
            // Position stats in top-left corner with padding
            ImVec2 stats_pos = ImVec2(
                canvas_pos.x + padding.x, 
                canvas_pos.y + padding.y
            );
            
            // Draw semi-transparent background
            draw_list->AddRectFilled(
                ImVec2(stats_pos.x - padding.x, stats_pos.y - padding.y),
                ImVec2(stats_pos.x + text_size.x + padding.x, 
                    stats_pos.y + text_size.y + padding.y),
                IM_COL32(0, 0, 0, 180)
            );

            // Draw statistics text
            draw_list->AddText(stats_pos, IM_COL32(255, 255, 255, 255), stats_buf);

            // Draw graph label with background - moved to top-right
            ImVec2 label_size = ImGui::CalcTextSize(label);
            ImVec2 label_pos = ImVec2(
                canvas_pos.x + canvas_size.x - label_size.x - padding.x * 2,
                canvas_pos.y + padding.y
            );
            
            // Draw label background
            draw_list->AddRectFilled(
                ImVec2(label_pos.x - padding.x, label_pos.y - padding.y),
                ImVec2(label_pos.x + label_size.x + padding.x, 
                    label_pos.y + label_size.y + padding.y),
                IM_COL32(0, 0, 0, 180)
            );

            draw_list->AddText(label_pos, IM_COL32(255, 255, 255, 255), label);
        }

        ImGui::PopID();
    }
}