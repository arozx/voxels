#pragma once

#include <pch.h>
#include "../Core/Transform.h"
#include "../Window/Window.h"
#include "../Renderer/Renderer.h"

namespace Engine {
    class ImGuiOverlay {
    public: 
        ImGuiOverlay(Window* window);
        ~ImGuiOverlay() = default;

        void OnRender(const Transform& transform, bool& showFPSCounter, float currentFPS, 
            float avgFPS, float frameTime, float fps1Low, float fps1High);
        void RenderTransformControls(Transform& transform);
        void RenderProfiler();
        void RenderRendererSettings();

    private:
        Window* m_Window;
        Renderer* m_Renderer;
    };
}