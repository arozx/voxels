#pragma once
#include "Window/Window.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace Engine {
    class InputSystem {
    public:
        InputSystem(Window* window, Renderer& renderer);
        
        void Update(float deltaTime);
        void OnEvent(Event& e);
        
        bool IsKeyPressed(int keycode) const;
        bool IsMouseButtonPressed(int button) const;
        std::pair<float, float> GetMousePosition() const;
        
        void EnableMouseControl(bool enable);
        bool IsMouseControlEnabled() const { return m_MouseControlEnabled; }
        
        void SetSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
        float GetSensitivity() const { return m_MouseSensitivity; }

    private:
        void HandleMouseMovement(const MouseMovedEvent& e);
        void HandleKeyInput(float deltaTime);
        void HandleCameraMovement(float deltaTime);
        
        Window* m_Window;
        Renderer& m_Renderer;
        
        bool m_MouseControlEnabled = false;
        bool m_FirstMouse = true;
        float m_LastMouseX = 0.0f;
        float m_LastMouseY = 0.0f;
        float m_MouseSensitivity = 0.1f;
    };
}