#pragma once
#include "Window/Window.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace Engine {
    /**
     * @brief Handles all input processing for the application
     * 
     * Manages keyboard and mouse input, camera controls, and input event handling.
     * Provides an interface between GLFW input and the game systems.
     */
    class InputSystem {
    public:
        /**
         * @brief Constructs an input system
         * @param window Pointer to the application window
         * @param renderer Reference to the renderer
         */
        InputSystem(Window* window, Renderer& renderer);
        
        /**
         * @brief Updates input state and handles continuous input
         * @param deltaTime Time elapsed since last update
         */
        void Update(float deltaTime);

        /**
         * @brief Processes input events
         * @param e Event to process
         */
        void OnEvent(Event& e);
        
        /**
         * @brief Checks if a key is currently pressed
         * @param keycode GLFW key code to check
         * @return true if key is pressed
         */
        
        bool IsKeyPressed(int keycode) const;

        /**
         * @brief Checks if a mouse button is currently pressed
         * @param button GLFW mouse button code to check
         * @return true if button is pressed
         */
        bool IsMouseButtonPressed(int button) const;

        /**
         * @brief Gets the current mouse cursor position
         * @return Pair of x,y coordinates
         */
        std::pair<float, float> GetMousePosition() const;
        
        /**
         * @brief Enables or disables mouse control mode
         * @param enable true to enable mouse control
         */
        void EnableMouseControl(bool enable);

        /** @return Whether mouse control is currently enabled */
        bool IsMouseControlEnabled() const { return m_MouseControlEnabled; }
        
        /**
         * @brief Sets mouse sensitivity
         * @param sensitivity New sensitivity value
         */
        void SetSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }

        /** @return Current mouse sensitivity */
        float GetSensitivity() const { return m_MouseSensitivity; }

    private:
        /**
         * @brief Handles mouse movement input
         * @param e Mouse moved event
         */
        void HandleMouseMovement(const MouseMovedEvent& e);

        /**
         * @brief Processes keyboard input
         * @param deltaTime Time elapsed since last update
         */
        void HandleKeyInput(float deltaTime);

        /**
         * @brief Updates camera position based on input
         * @param deltaTime Time elapsed since last update
         */
        void HandleCameraMovement(float deltaTime);
        
        Window* m_Window;              ///< Pointer to application window
        Renderer& m_Renderer;          ///< Reference to renderer
        
        bool m_MouseControlEnabled = false;  ///< Mouse control mode state
        bool m_FirstMouse = true;           ///< First mouse input flag
        float m_LastMouseX = 0.0f;          ///< Last mouse X position
        float m_LastMouseY = 0.0f;          ///< Last mouse Y position
        float m_MouseSensitivity = 0.1f;    ///< Mouse movement sensitivity
    };
}