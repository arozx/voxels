/**
 * @file InputSystem.cpp
 * @brief Implementation of the input processing system
 * 
 * Handles keyboard and mouse input processing, camera controls,
 * and input event handling for the application.
 */
#include "Window/Window.h"
#include "Renderer/Renderer.h"
#include "Events/Event.h"
#include "Events/MouseEvent.h"
#include "Events/KeyCodes.h"
#include "InputSystem.h"
#include "Events/KeyCodes.h"
#include <GLFW/glfw3.h>

namespace Engine {
    InputSystem::InputSystem(Window* window, Renderer& renderer)
        : m_Window(window), m_Renderer(renderer) {
    }

    void InputSystem::Update(float deltaTime) {
        // Handle mouse control toggle
        if (IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2)) {
            if (!m_MouseControlEnabled) {
                EnableMouseControl(true);
            }
        } else {
            EnableMouseControl(false);
        }

        HandleKeyInput(deltaTime);
        HandleCameraMovement(deltaTime);
    }

    void InputSystem::HandleKeyInput(float deltaTime) {
        // Camera type switching
        if (IsKeyPressed(GLFW_KEY_1)) {
            m_Renderer.SetCameraType(Renderer::CameraType::Orthographic);
        }
        if (IsKeyPressed(GLFW_KEY_2)) {
            m_Renderer.SetCameraType(Renderer::CameraType::Perspective);
        }
    }

    void InputSystem::HandleCameraMovement(float deltaTime) {
        if (m_Renderer.GetCameraType() == Renderer::CameraType::Orthographic) {
            if (IsKeyPressed(GLFW_KEY_W)) m_Renderer.GetCamera()->MoveUp(deltaTime);
            if (IsKeyPressed(GLFW_KEY_S)) m_Renderer.GetCamera()->MoveDown(deltaTime);
            if (IsKeyPressed(GLFW_KEY_A)) m_Renderer.GetCamera()->MoveLeft(deltaTime);
            if (IsKeyPressed(GLFW_KEY_D)) m_Renderer.GetCamera()->MoveRight(deltaTime);
        } else {
            if (IsKeyPressed(GLFW_KEY_W)) m_Renderer.GetPerspectiveCamera()->MoveForward(deltaTime);
            if (IsKeyPressed(GLFW_KEY_S)) m_Renderer.GetPerspectiveCamera()->MoveBackward(deltaTime);
            if (IsKeyPressed(GLFW_KEY_A)) m_Renderer.GetPerspectiveCamera()->MoveLeft(deltaTime);
            if (IsKeyPressed(GLFW_KEY_D)) m_Renderer.GetPerspectiveCamera()->MoveRight(deltaTime);
            if (IsKeyPressed(GLFW_KEY_SPACE)) m_Renderer.GetPerspectiveCamera()->MoveUp(deltaTime);
            if (IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) m_Renderer.GetPerspectiveCamera()->MoveDown(deltaTime);
        }
    }

    void InputSystem::OnEvent(Event& e) {
        if (e.GetEventType() == EventType::MouseMoved) {
            if (m_MouseControlEnabled) {
                HandleMouseMovement(static_cast<const MouseMovedEvent&>(e));
            }
        }
    }

    void InputSystem::HandleMouseMovement(const MouseMovedEvent& e) {
        if (m_FirstMouse) {
            m_LastMouseX = e.GetX();
            m_LastMouseY = e.GetY();
            m_FirstMouse = false;
            return;
        }

        float xOffset = e.GetX() - m_LastMouseX;
        float yOffset = m_LastMouseY - e.GetY();

        m_LastMouseX = e.GetX();
        m_LastMouseY = e.GetY();

        if (m_Renderer.GetCameraType() == Renderer::CameraType::Perspective) {
            m_Renderer.GetPerspectiveCamera()->RotateWithMouse(xOffset, yOffset, m_MouseSensitivity);
        }
    }

    void InputSystem::EnableMouseControl(bool enable) {
        m_MouseControlEnabled = enable;
        m_FirstMouse = true;
        GLFWwindow* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
        glfwSetInputMode(window, GLFW_CURSOR, 
            enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    bool InputSystem::IsKeyPressed(int keycode) const {
        GLFWwindow* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
        return glfwGetKey(window, keycode) == GLFW_PRESS;
    }

    bool InputSystem::IsMouseButtonPressed(int button) const {
        GLFWwindow* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    std::pair<float, float> InputSystem::GetMousePosition() const {
        GLFWwindow* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return {static_cast<float>(x), static_cast<float>(y)};
    }
}