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
        // Lock cursor on startup
        m_CursorLocked = true;
        UpdateCursorState();
    }

    void InputSystem::Update(float deltaTime) {
        HandleKeyInput(deltaTime);
        
        // Accumulate time for movement updates
        m_MovementAccumulator += deltaTime;
        
        // Update movement at fixed intervals
        while (m_MovementAccumulator >= FIXED_TIMESTEP) {
            HandleCameraMovement(FIXED_TIMESTEP);
            m_MovementAccumulator -= FIXED_TIMESTEP;
        }
    }

    void InputSystem::HandleKeyInput(float deltaTime) {
        // Add ESC key handling at the start
        bool currentEscPressed = IsKeyPressed(GLFW_KEY_ESCAPE);
        if (currentEscPressed && !m_LastEscPressed) {
            m_CursorLocked = !m_CursorLocked;
            UpdateCursorState();
            ToggleMovementLock();
        }
        m_LastEscPressed = currentEscPressed;

        // Camera type switching
        if (IsKeyPressed(GLFW_KEY_1)) {
            m_Renderer.SetCameraType(Renderer::CameraType::Orthographic);
        }
        if (IsKeyPressed(GLFW_KEY_2)) {
            m_Renderer.SetCameraType(Renderer::CameraType::Perspective);
        }
        
        // Camera control toggles with state tracking
        bool currentVPressed = IsKeyPressed(GLFW_KEY_V);
        if (currentVPressed && !m_LastVPressed) {
            ToggleCameraControls();
        }
        m_LastVPressed = currentVPressed;

        bool currentBPressed = IsKeyPressed(GLFW_KEY_B);
        if (currentBPressed && !m_LastBPressed) {
            ToggleSmoothCamera();
        }
        m_LastBPressed = currentBPressed;
        
        // Sensitivity adjustment
        HandleSensitivityAdjustment();
    }

    void InputSystem::HandleSensitivityAdjustment() {
        if (IsKeyPressed(GLFW_KEY_RIGHT_BRACKET)) {
            m_MouseSensitivity += 0.01f;
        }
        if (IsKeyPressed(GLFW_KEY_LEFT_BRACKET)) {
            m_MouseSensitivity = std::max(0.01f, m_MouseSensitivity - 0.01f);
        }
    }

    void InputSystem::HandleSpeedModifiers(float& speed) {
        if (IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
            speed *= m_SlowMultiplier;
        } else if (IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
            speed *= m_SprintMultiplier;
        }
    }

    void InputSystem::HandleCameraMovement(float deltaTime) {
        if (!m_CameraEnabled || m_MovementLocked) return;
        
        auto camera = m_Renderer.GetCamera();
        if (!camera) return;

        float speed = m_MovementSpeed;
        HandleSpeedModifiers(speed);
        
        if (m_Renderer.GetCameraType() == Renderer::CameraType::Perspective) {
            auto perspCamera = m_Renderer.GetPerspectiveCamera();
            if (perspCamera) {
                glm::vec3 moveDir(0.0f);
                
                // Get camera orientation vectors
                glm::vec3 forward = glm::normalize(glm::vec3(perspCamera->GetFront().x, 0.0f, perspCamera->GetFront().z));
                glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
                
                // Move relative to camera orientation
                if (IsKeyPressed(GLFW_KEY_W)) moveDir += forward;
                if (IsKeyPressed(GLFW_KEY_S)) moveDir -= forward;
                if (IsKeyPressed(GLFW_KEY_A)) moveDir -= right;
                if (IsKeyPressed(GLFW_KEY_D)) moveDir += right;
                if (IsKeyPressed(GLFW_KEY_SPACE)) moveDir.y += 1.0f;
                if (IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) moveDir.y -= 1.0f;

                if (glm::length(moveDir) > 0.0f) {
                    moveDir = glm::normalize(moveDir) * speed * deltaTime;
                    
                    if (m_SmoothCamera) {
                        // Use position directly from PerspectiveCamera
                        const glm::vec3& currentPos = perspCamera->GetPosition();
                        m_TargetPosition = currentPos + moveDir;
                        glm::vec3 newPos = currentPos;
                        SmoothDamp(newPos, m_TargetPosition, m_CurrentVelocity, m_SmoothTime, deltaTime);
                        perspCamera->SetPosition(newPos);
                    } else {
                        perspCamera->SetPosition(perspCamera->GetPosition() + moveDir);
                    }
                }
            }
        }
    }

    void InputSystem::SmoothDamp(glm::vec3& current, const glm::vec3& target,
            glm::vec3& velocity, float smoothTime, float deltaTime) {
        float omega = 2.0f / smoothTime;
        float x = omega * deltaTime;
        float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
        
        glm::vec3 change = current - target;
        glm::vec3 temp = (velocity + omega * change) * deltaTime;
        
        velocity = (velocity - omega * temp) * exp;
        current = target + (change + temp) * exp;
    }

    void InputSystem::OnEvent(Event& e) {
        if (e.GetEventType() == EventType::MouseMoved) {
            HandleMouseMovement(static_cast<const MouseMovedEvent&>(e));
        }
    }

    void InputSystem::HandleMouseMovement(const MouseMovedEvent& e) {
        if (!m_CameraEnabled || m_MovementLocked) return;
        
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

        // Only rotate perspective camera
        if (m_Renderer.GetCameraType() == Renderer::CameraType::Perspective) {
            if (auto perspCamera = m_Renderer.GetPerspectiveCamera()) {
                perspCamera->RotateWithMouse(xOffset, yOffset, m_MouseSensitivity);
            }
        }
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

    void InputSystem::UpdateCursorState() {
        GLFWwindow* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
        if (m_CursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}