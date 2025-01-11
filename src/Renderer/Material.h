#pragma once
#include "../pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../Shader/Shader.h"
#include "Texture.h"

namespace Engine {
    class Material {
    public:
        Material(std::shared_ptr<Shader> shader);
        ~Material() = default;

        void Bind();
        void Unbind();

        void SetFloat(const std::string& name, float value);
        void SetInt(const std::string& name, int value);
        void SetBool(const std::string& name, bool value);
        void SetVector2(const std::string& name, const glm::vec2& value);
        void SetVector3(const std::string& name, const glm::vec3& value);
        void SetVector4(const std::string& name, const glm::vec4& value);
        void SetMatrix4(const std::string& name, const glm::mat4& value);
        void SetTexture(const std::string& name, const std::shared_ptr<Texture>& texture);

        std::shared_ptr<Shader> GetShader() const { return m_Shader; }

    private:
        std::shared_ptr<Shader> m_Shader;
        std::unordered_map<std::string, glm::vec4> m_VectorProperties;
        std::unordered_map<std::string, float> m_FloatProperties;
        std::unordered_map<std::string, int> m_IntProperties;
        std::unordered_map<std::string, bool> m_BoolProperties;
        std::unordered_map<std::string, glm::mat4> m_MatrixProperties;
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
        uint32_t m_TextureSlot = 0;
    };
}
