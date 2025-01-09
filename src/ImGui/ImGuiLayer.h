#pragma once
#include "../Window/Window.h"

namespace Engine {
    class ImGuiLayer {
    public:
        ImGuiLayer() = default;
        ~ImGuiLayer() = default;

        void Init(Window* window);
        void Shutdown();
        
        void Begin();
        void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }
        bool AreEventsBlocked() const { return m_BlockEvents; }

    private:
        bool m_BlockEvents = true;
        Window* m_Window = nullptr;
    };
}