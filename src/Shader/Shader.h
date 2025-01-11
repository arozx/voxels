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

    private:
        uint32_t m_Program;
        bool CompileShader(const char* source, uint32_t type, uint32_t& shader);
    };
}