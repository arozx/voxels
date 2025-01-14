#pragma once

#include "../Window/Window.h"

namespace Engine {
    /**
     * @brief Manages Dear ImGui integration with the application
     * 
     * Handles initialization, shutdown, and frame-by-frame rendering
     * of Dear ImGui interface elements.
     */
    class ImGuiLayer {
    public:
        ImGuiLayer() = default;
        ~ImGuiLayer() = default;

        /**
         * @brief Initializes ImGui context and bindings
         * @param window Pointer to the application window
         */
        void Init(Window* window);

        /** @brief Cleans up ImGui resources */
        void Shutdown();
        
        /** @brief Begins a new ImGui frame */
        void Begin();

        /** @brief Ends the current ImGui frame and renders */
        void End();

        /**
         * @brief Controls whether ImGui should block events from reaching the application
         * @param block True to block events, false to pass them through
         */
        void BlockEvents(bool block) { m_BlockEvents = block; }

        /** @return Whether ImGui is currently blocking events */
        bool AreEventsBlocked() const { return m_BlockEvents; }

    private:
        bool m_BlockEvents = true;     ///< Whether ImGui should block events
        Window* m_Window = nullptr;    ///< Pointer to the application window
    };
}