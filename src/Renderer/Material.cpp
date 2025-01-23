/**
 * @file Material.cpp
 * @brief Implementation of the material system
 * 
 * Handles shader parameter management and texture binding for materials.
 */
#include "Material.h"
#include "../Core/AssetManager.h"
#include "../Shader/Shader.h"
#include "Texture.h"

namespace Engine {
    Material::Material(std::shared_ptr<Shader> shader)
        : m_Shader(shader) {
        // Use cached shader if available
        std::string shaderPath = shader->GetPath(); // Need to add this getter
        if (auto cachedShader = AssetManager::Get().LoadResource<Shader>(shaderPath)) {
            m_Shader = cachedShader;
        }
    }

    /**
     * @brief Applies all material properties to the shader
     * 
     * Binds the shader and sets all stored uniform values and textures.
     */
    void Material::Bind() {
        m_Shader->Bind();
        
        // Apply all properties
        for (const auto& [name, value] : m_FloatProperties)
            m_Shader->SetFloat(name, value);
            
        for (const auto& [name, value] : m_IntProperties)
            m_Shader->SetInt(name, value);
            
        for (const auto& [name, value] : m_VectorProperties)
            m_Shader->SetVector4(name, value);
            
        for (const auto& [name, value] : m_MatrixProperties)
            m_Shader->SetMat4(name, value);
            
        // Bind textures
        m_TextureSlot = 0;
        for (const auto& [name, texture] : m_Textures) {
            texture->Bind(m_TextureSlot);
            m_Shader->SetInt(name, m_TextureSlot);
            m_TextureSlot++;
        }
    }

    /** @brief Unbinds the material's shader */
    void Material::Unbind() {
        m_Shader->Unbind();
    }

    /** @brief Sets a float property */
    void Material::SetFloat(const std::string& name, float value) {
        m_FloatProperties[name] = value;
    }

    /** @brief Sets an int property */
    void Material::SetInt(const std::string& name, int value) {
        m_IntProperties[name] = value;
    }

    /** @brief Sets a bool property */
    void Material::SetBool(const std::string& name, bool value) {
        m_IntProperties[name] = value ? 1 : 0;
    }

    /** @brief Sets a vec2 property */
    void Material::SetVector2(const std::string& name, const glm::vec2& value) {
        m_VectorProperties[name] = glm::vec4(value.x, value.y, 0.0f, 0.0f);
    }

    /** @brief Sets a vec3 property */
    void Material::SetVector3(const std::string& name, const glm::vec3& value) {
        m_VectorProperties[name] = glm::vec4(value, 0.0f);
    }

    /** @brief Sets a vec4 property */
    void Material::SetVector4(const std::string& name, const glm::vec4& value) {
        m_VectorProperties[name] = value;
    }

    /** @brief Sets a mat4 property */
    void Material::SetMatrix4(const std::string& name, const glm::mat4& value) {
        m_MatrixProperties[name] = value;
    }

    /** @brief Sets a texture property */
    void Material::SetTexture(const std::string& name, const std::shared_ptr<Texture>& texture) {
        m_Textures[name] = texture;
    }
}
