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

/**
 * @brief ImGui-based overlay for engine debugging and control interfaces.
 *
 * Provides a collection of ImGui windows and panels to display performance metrics,
 * manipulate object transforms, adjust renderer settings, debug events, and control terrain generation.
 */

/**
 * @brief Initializes the ImGui overlay.
 *
 * Sets up the ImGui overlay using the specified application window, which serves as the rendering context for ImGui elements.
 *
 * @param window Pointer to the application window.
 */

/**
 * @brief Default destructor.
 */

/**
 * @brief Renders the performance statistics overlay.
 *
 * Displays real-time performance metrics including current FPS, average FPS, frame time,
 * and the 1% low/high FPS values. Optionally shows the FPS counter based on the provided flag.
 *
 * @param renderObject Current render object context.
 * @param showFPSCounter Whether the FPS counter should be displayed.
 * @param currentFPS Current frames per second.
 * @param averageFPS Average frames per second.
 * @param frameTime Time taken to render the last frame.
 * @param fps1PercentLow 1% low FPS value.
 * @param fps1PercentHigh 1% high FPS value.
 */

/**
 * @brief Renders the transform manipulation controls.
 *
 * Displays interactive controls for adjusting the transform properties of a render object,
 * such as translation, rotation, and scale.
 *
 * @param renderObject The render object whose transform is being controlled.
 */

/**
 * @brief Displays the profiler information window.
 *
 * Renders a window with detailed profiling metrics for performance analysis.
 * This method is const and does not modify the overlay state.
 */

/**
 * @brief Displays the renderer settings panel.
 *
 * Provides a UI panel to adjust various renderer options and optimize visual performance.
 */

/**
 * @brief Displays the event debugging interface.
 *
 * Renders a window that allows in-depth inspection and debugging of engine events.
 */

/**
 * @brief Displays the terrain generation controls.
 *
 * Renders interactive UI elements for adjusting terrain generation parameters,
 * enabling real-time debugging and refinement.
 */

/**
 * @brief Sets the visibility of transform manipulation controls.
 *
 * @param show Flag indicating whether the transform controls should be displayed.
 */

/**
 * @brief Sets the visibility of the profiler window.
 *
 * @param show Flag indicating whether the profiler should be displayed.
 */

/**
 * @brief Sets the visibility of the renderer settings panel.
 *
 * @param show Flag indicating whether the renderer settings should be displayed.
 */

/**
 * @brief Sets the visibility of the event debugger interface.
 *
 * @param show Flag indicating whether the event debugger should be displayed.
 */

/**
 * @brief Sets the visibility of the terrain controls.
 *
 * @param show Flag indicating whether the terrain controls should be displayed.
 */

/**
 * @brief Sets the visibility of the FPS counter overlay.
 *
 * @param show Flag indicating whether the FPS counter should be displayed.
 */
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
        void RenderProfiler() const;
        /** @brief Renders renderer settings panel */
        void RenderRendererSettings();
        /** @brief Renders event debugging window */
        void RenderEventDebugger();
        /** @brief Renders terrain generation controls */
        void RenderTerrainControls();

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