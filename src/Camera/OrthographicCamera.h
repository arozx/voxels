#pragma once
#include "../pch.h"

namespace Engine {
    class OrthographicCamera {
    public:
        OrthographicCamera(float left, float right, float bottom, float top)
            : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), 
            m_ViewMatrix(1.0f)
        {
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

        void SetPosition(const glm::vec3& position) { 
            m_Position = position; 
            RecalculateViewMatrix();
        }
        void SetRotation(float rotation) { 
            m_Rotation = rotation; 
            RecalculateViewMatrix();
        }

        const glm::vec3& GetPosition() const { return m_Position; }
        float GetRotation() const { return m_Rotation; }

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        void MoveRight(float deltaTime) { 
            m_Position.x += m_MovementSpeed * deltaTime; 
            RecalculateViewMatrix();
        }
        
        void MoveLeft(float deltaTime) { 
            m_Position.x -= m_MovementSpeed * deltaTime; 
            RecalculateViewMatrix();
        }
        
        void MoveUp(float deltaTime) { 
            m_Position.y += m_MovementSpeed * deltaTime; 
            RecalculateViewMatrix();
        }
        
        void MoveDown(float deltaTime) { 
            m_Position.y -= m_MovementSpeed * deltaTime; 
            RecalculateViewMatrix();
        }

        void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
        float GetMovementSpeed() const { return m_MovementSpeed; }

        void RotateWithMouse(float xOffset, float yOffset, float deltaTime) {
            m_Rotation += xOffset * m_RotationSpeed * deltaTime;
            RecalculateViewMatrix();
        }

        void SetRotationSpeed(float speed) { m_RotationSpeed = speed; }
        float GetRotationSpeed() const { return m_RotationSpeed; }

    private:
        void RecalculateViewMatrix() {
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

            m_ViewMatrix = glm::inverse(transform);
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float m_Rotation = 0.0f;
        float m_MovementSpeed = 5.0f;
        float m_RotationSpeed = 50.0f;  // Degrees per second
    };
}