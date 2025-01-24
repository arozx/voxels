#pragma once

#include <pch.h>
#include "../Core/Transform.h"
#include "../Renderer/RenderObject.h"
#include "../Window/Window.h"
#include "../Renderer/Renderer.h"
#include "../Events/EventDebugger.h"
#include "../TerrainSystem/TerrainSystem.h"
#include "../Core/FPSCounter.h"
#include "ImGuiFlameGraph.h"

namespace Engine {
    /**
     * @brief ImGui-based overlay for debug and control interfaces
     * 
     * Provides various debug windows and control panels for the engine,
     * including performance metrics, transform controls, and debug info.
     */
    class ImGuiOverlay {
    public: 
        /**
         * @brief Constructs the ImGui overlay
         * @param window Pointer to the application window
         */
        ImGuiOverlay(Window* window);
        ~ImGuiOverlay() = default;

        /**
         * @brief Renders performance statistics overlay
         * @param renderObject Current render object
         * @param showFPSCounter Whether to show FPS counter
         * @param currentFPS Current frames per second
         * @param averageFPS Average frames per second
         * @param frameTime Time taken for last frame
         * @param fps1PercentLow 1% low FPS value
         * @param fps1PercentHigh 1% high FPS value
         */
        void OnRender(RenderObject& renderObject, bool showFPSCounter, 
            float currentFPS, float averageFPS, float frameTime, 
            float fps1PercentLow, float fps1PercentHigh);

        /** @brief Renders transform manipulation controls */
        void RenderTransformControls(RenderObject& renderObject);
        /** @brief Renders profiler information window */
        void RenderProfiler();
        /** @brief Renders renderer settings panel */
        void RenderRendererSettings();
        /** @brief Renders event debugging window */
        void RenderEventDebugger();
        /** @brief Renders terrain generation controls */
        void RenderTerrainControls(TerrainSystem& terrainSystem);

        // Window visibility controls
        void ShowTransformControls(bool show) { m_ShowTransformControls = show; }
        void ShowProfiler(bool show) { m_ShowProfiler = show; }
        void ShowRendererSettings(bool show) { m_ShowRendererSettings = show; }
        void ShowEventDebugger(bool show) { m_ShowEventDebugger = show; }
        void ShowTerrainControls(bool show) { m_ShowTerrainControls = show; }
        void ShowFPSCounter(bool show) { m_ShowFPSCounter = show; }

    private:
        Window* m_Window;                                               ///< Pointer to application window
        Renderer* m_Renderer;                                           ///< Pointer to renderer
        bool m_ShowEventDebugger = true;                                ///< Event debugger visibility flag
        FPSCounter m_FPSCounter;                                        ///< FPS counter
        ImGuiWidgetFlameGraph::FlameGraphSettings m_FlameGraphSettings; ///< Flame graph settings
        bool m_ShowTransformControls = true;
        bool m_ShowProfiler = true;
        bool m_ShowRendererSettings = true;
        bool m_ShowTerrainControls = true;
        bool m_ShowFPSCounter = true;
    };
}