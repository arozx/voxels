#pragma once

#include <pch.h>

namespace Engine {
    /**
     * @brief An orthographic camera class for 2D rendering
     * 
     * Implements an orthographic camera with position and rotation controls.
     * Suitable for 2D games and UI rendering.
     */
    class OrthographicCamera {
    public:
        /**
         * @brief Constructs an orthographic camera
         * @param left Left plane coordinate
         * @param right Right plane coordinate
         * @param bottom Bottom plane coordinate
         * @param top Top plane coordinate
         */
        OrthographicCamera(float left, float right, float bottom, float top)
            : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), 
            m_ViewMatrix(1.0f)
        {
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

        /**
         * @brief Sets the camera position
         * @param position New position vector
         */
        void SetPosition(const glm::vec3& position) { 
            m_Position = position; 
            RecalculateViewMatrix();
        }

        /**
         * @brief Sets the camera rotation around Z-axis
         * @param rotation Rotation angle in degrees
         */
        void SetRotation(float rotation) { 
            m_Rotation = rotation; 
            RecalculateViewMatrix();
        }

        /** @return Current camera position */
        const glm::vec3& GetPosition() const { return m_Position; }
        /** @return Current rotation angle in degrees */
        float GetRotation() const { return m_Rotation; }

        /** @return The current projection matrix */
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        /** @return The current view matrix */
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        /** @return The combined view-projection matrix */
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        /** @brief Moves the camera right */
        void MoveRight(float deltaTime) { 
            m_Position.x += m_MovementSpeed * deltaTime; 
            RecalculateViewMatrix();
        }
        
        /** @brief Moves the camera left */
        void MoveLeft(float deltaTime) { 
            m_Position.x -= m_MovementSpeed * deltaTime; 
            RecalculateViewMatrix();
        }
        
        /** @brief Moves the camera up */
        void MoveUp(float deltaTime) { 
            m_Position.y += m_MovementSpeed * deltaTime; 
            RecalculateViewMatrix();
        }
        
        /** @brief Moves the camera down */
        void MoveDown(float deltaTime) { 
            m_Position.y -= m_MovementSpeed * deltaTime; 
            RecalculateViewMatrix();
        }


        /**
         * @brief Sets the camera movement speed
         * @param speed New movement speed value
         */
        void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
        /** @return Current movement speed */
        float GetMovementSpeed() const { return m_MovementSpeed; }

        /**
         * @brief Rotates the camera based on mouse movement
         * @param xOffset Mouse X movement delta
         * @param yOffset Mouse Y movement delta
         * @param deltaTime Time since last frame
         */
        void RotateWithMouse(float xOffset, float yOffset, float deltaTime) {
            m_Rotation += xOffset * m_RotationSpeed * deltaTime;
            RecalculateViewMatrix();
        }

        /**
         * @brief Sets the camera rotation speed
         * @param speed New rotation speed in degrees per second
         */
        void SetRotationSpeed(float speed) { m_RotationSpeed = speed; }
        /** @return Current rotation speed in degrees per second */
        float GetRotationSpeed() const { return m_RotationSpeed; }

    private:
        /** @brief Recalculates view matrix after position/rotation changes */
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
