#include "Material.h"

namespace Engine {
    Material::Material(std::shared_ptr<Shader> shader)
        : m_Shader(shader) {
    }

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
    }

    void Material::Unbind() {
        m_Shader->Unbind();
    }

    void Material::SetFloat(const std::string& name, float value) {
        m_FloatProperties[name] = value;
    }

    void Material::SetInt(const std::string& name, int value) {
        m_IntProperties[name] = value;
    }

    void Material::SetBool(const std::string& name, bool value) {
        m_IntProperties[name] = value ? 1 : 0;
    }

    void Material::SetVector2(const std::string& name, const glm::vec2& value) {
        m_VectorProperties[name] = glm::vec4(value.x, value.y, 0.0f, 0.0f);
    }

    void Material::SetVector3(const std::string& name, const glm::vec3& value) {
        m_VectorProperties[name] = glm::vec4(value, 0.0f);
    }

    void Material::SetVector4(const std::string& name, const glm::vec4& value) {
        m_VectorProperties[name] = value;
    }

    void Material::SetMatrix4(const std::string& name, const glm::mat4& value) {
        m_MatrixProperties[name] = value;
    }
}
