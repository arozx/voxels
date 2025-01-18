#pragma once
#include <glm/glm.hpp>

namespace Engine {
    /** @brief Class representing a light source in the scene
     *
     *  Light contains properties for position, color, and various lighting parameters
     *  used in lighting calculations.
     */
    class Light {
    public:
        /** @brief Constructor for creating a light source
         *  @param position The position of the light in 3D space
         *  @param color The color of the light
         *  @param ambientStrength The strength of ambient lighting
         *  @param specularStrength The strength of specular highlights
         *  @param shininess The shininess factor for specular calculations
         */
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

        /** @brief Sets the position of the light
         *  @param position New position in 3D space
         */
        void SetPosition(const glm::vec3& position) { m_Position = position; }

        /** @brief Sets the color of the light
         *  @param color New color value
         */
        void SetColor(const glm::vec3& color) { m_Color = color; }

        /** @brief Sets the ambient strength
         *  @param strength New ambient strength value
         */
        void SetAmbientStrength(float strength) { m_AmbientStrength = strength; }

        /** @brief Sets the specular strength
         *  @param strength New specular strength value
         */
        void SetSpecularStrength(float strength) { m_SpecularStrength = strength; }

        /** @brief Sets the shininess factor
         *  @param shininess New shininess value
         */
        void SetShininess(float shininess) { m_Shininess = shininess; }

        /** @brief Gets the position of the light
         *  @return Current position
         */
        const glm::vec3& GetPosition() const { return m_Position; }

        /** @brief Gets the color of the light
         *  @return Current color
         */
        const glm::vec3& GetColor() const { return m_Color; }

        /** @brief Gets the ambient strength
         *  @return Current ambient strength
         */
        float GetAmbientStrength() const { return m_AmbientStrength; }

        /** @brief Gets the specular strength
         *  @return Current specular strength
         */
        float GetSpecularStrength() const { return m_SpecularStrength; }

        /** @brief Gets the shininess factor
         *  @return Current shininess value
         */
        float GetShininess() const { return m_Shininess; }

    private:
        glm::vec3 m_Position;        ///< Position of the light in 3D space
        glm::vec3 m_Color;           ///< Color of the light
        float m_AmbientStrength;     ///< Strength of ambient lighting
        float m_SpecularStrength;    ///< Strength of specular highlights
        float m_Shininess;           ///< Shininess factor for specular calculations
    };
}