#pragma once
#include "../pch.h"
#include <glm/gtc/type_ptr.hpp>

namespace Engine {
    class Shader {
    public:
        Shader(const char* vertexSrc, const char* fragmentSrc);
        ~Shader();

        void Bind() const;
        void Unbind() const;
        uint32_t GetProgram() const { return m_Program; }
        
        void SetMat4(const std::string& name, const glm::mat4& matrix);
        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetVector2(const std::string& name, const glm::vec2& value);
        void SetVector3(const std::string& name, const glm::vec3& value);
        void SetVector4(const std::string& name, const glm::vec4& value);

    private:
        uint32_t m_Program;
        bool CompileShader(const char* source, uint32_t type, uint32_t& shader);
    };
}