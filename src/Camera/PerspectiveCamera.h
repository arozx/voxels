#pragma once

#include <pch.h>

namespace Engine {
    /**
     * @brief A perspective camera class for 3D rendering
     * 
     * Implements a perspective camera with position, rotation and movement controls.
     * Handles view and projection matrix calculations for 3D scene rendering.
     */
    class PerspectiveCamera {
    public:
        /**
         * @brief Constructs a perspective camera
         * @param fov Field of view in degrees
         * @param aspectRatio Aspect ratio of the viewport (width/height)
         * @param nearClip Distance to near clipping plane
         * @param farClip Distance to far clipping plane
         */
        PerspectiveCamera(float fov = 45.0f, float aspectRatio = 16.0f/9.0f, float nearClip = 0.1f, float farClip = 100.0f);
        
        /**
         * @brief Sets the camera position
         * @param position New position vector
         */
        void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

        /**
         * @brief Gets the camera position
         * @return Current position vector
         */
        const glm::vec3& GetPosition() const { return m_Position; }

        /**
         * @brief Sets the camera rotation
         * @param pitch Rotation around X-axis in degrees
         * @param yaw Rotation around Y-axis in degrees
         */
        void SetRotation(float pitch, float yaw) { m_Pitch = pitch; m_Yaw = yaw; RecalculateViewMatrix(); }
        
        /** @brief Moves the camera forward based on current orientation */
        void MoveForward(float deltaTime);
        /** @brief Moves the camera backward based on current orientation */
        void MoveBackward(float deltaTime);
        /** @brief Moves the camera left based on current orientation */
        void MoveLeft(float deltaTime);
        /** @brief Moves the camera right based on current orientation */
        void MoveRight(float deltaTime);
        /** @brief Moves the camera up along world Y-axis */
        void MoveUp(float deltaTime);
        /** @brief Moves the camera down along world Y-axis */
        void MoveDown(float deltaTime);

        /**
         * @brief Rotates the camera based on mouse movement
         * @param xOffset Mouse X movement delta
         * @param yOffset Mouse Y movement delta
         * @param sensitivity Mouse movement sensitivity factor
         */
        void RotateWithMouse(float xOffset, float yOffset, float sensitivity);
        
        /** @return The current view matrix */
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        /** @return The current projection matrix */
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        /** @return The combined view-projection matrix */
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
        
    private:
        /** @brief Recalculates view matrix after position/rotation changes */
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