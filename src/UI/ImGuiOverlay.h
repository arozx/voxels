#pragma once

#include <pch.h>
#include "../Core/Transform.h"
#include "../Renderer/RenderObject.h"
#include "../Window/Window.h"
#include "../Renderer/Renderer.h"

namespace Engine {
    class ImGuiOverlay {
    public: 
        ImGuiOverlay(Window* window);
        ~ImGuiOverlay() = default;

        void OnRender(RenderObject& renderObject, bool showFPSCounter, 
            float currentFPS, float averageFPS, float frameTime, 
            float fps1PercentLow, float fps1PercentHigh);
        void RenderTransformControls(RenderObject& renderObject);
        void RenderProfiler();
        void RenderRendererSettings();

    private:
        Window* m_Window;
        Renderer* m_Renderer;
    };
}