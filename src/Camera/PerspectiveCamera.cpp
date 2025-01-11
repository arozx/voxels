#include "../pch.h"
#include "PerspectiveCamera.h"

namespace Engine {
    PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip) {
        m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::RecalculateViewMatrix() {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);
        
        m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
        
        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void PerspectiveCamera::MoveForward(float deltaTime) {
        m_Position += m_Front * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::MoveBackward(float deltaTime) {
        m_Position -= m_Front * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::MoveLeft(float deltaTime) {
        m_Position -= m_Right * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::MoveRight(float deltaTime) {
        m_Position += m_Right * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::MoveUp(float deltaTime) {
        m_Position += m_Up * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::MoveDown(float deltaTime) {
        m_Position -= m_Up * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::RotateWithMouse(float xOffset, float yOffset, float sensitivity) {
        m_Yaw += xOffset * sensitivity;
        m_Pitch += yOffset * sensitivity;

        if (m_Pitch > 89.0f) m_Pitch = 89.0f;
        if (m_Pitch < -89.0f) m_Pitch = -89.0f;

        RecalculateViewMatrix();
    }
}