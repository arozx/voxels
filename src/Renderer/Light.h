
#pragma once
#include <glm/glm.hpp>

namespace Engine {
    class Light {
    public:
        Light(const glm::vec3& position = glm::vec3(0.0f),
              const glm::vec3& color = glm::vec3(1.0f),
              float ambientStrength = 0.1f,
              float specularStrength = 0.5f,
              float shininess = 32.0f)
            : m_Position(position)
            , m_Color(color)
            , m_AmbientStrength(ambientStrength)
            , m_SpecularStrength(specularStrength)
            , m_Shininess(shininess) {}

        void SetPosition(const glm::vec3& position) { m_Position = position; }
        void SetColor(const glm::vec3& color) { m_Color = color; }
        void SetAmbientStrength(float strength) { m_AmbientStrength = strength; }
        void SetSpecularStrength(float strength) { m_SpecularStrength = strength; }
        void SetShininess(float shininess) { m_Shininess = shininess; }

        const glm::vec3& GetPosition() const { return m_Position; }
        const glm::vec3& GetColor() const { return m_Color; }
        float GetAmbientStrength() const { return m_AmbientStrength; }
        float GetSpecularStrength() const { return m_SpecularStrength; }
        float GetShininess() const { return m_Shininess; }

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Color;
        float m_AmbientStrength;
        float m_SpecularStrength;
        float m_Shininess;
    };
}