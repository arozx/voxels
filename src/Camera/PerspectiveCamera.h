
#pragma once
#include "../pch.h"

namespace Engine {
    class PerspectiveCamera {
    public:
        PerspectiveCamera(float fov = 45.0f, float aspectRatio = 16.0f/9.0f, float nearClip = 0.1f, float farClip = 100.0f);
        
        void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
        void SetRotation(float pitch, float yaw) { m_Pitch = pitch; m_Yaw = yaw; RecalculateViewMatrix(); }
        
        void MoveForward(float deltaTime);
        void MoveBackward(float deltaTime);
        void MoveLeft(float deltaTime);
        void MoveRight(float deltaTime);
        void MoveUp(float deltaTime);
        void MoveDown(float deltaTime);
        void RotateWithMouse(float xOffset, float yOffset, float sensitivity);
        
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
        
    private:
        void RecalculateViewMatrix();
        
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewProjectionMatrix;
        
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
        
        float m_Pitch = 0.0f;
        float m_Yaw = -90.0f;
        float m_MovementSpeed = 2.5f;
    };
}