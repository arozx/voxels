#pragma once
#include "../pch.h"

namespace Engine {
    class Shader {
    public:
        Shader(const char* vertexSrc, const char* fragmentSrc);
        ~Shader();

        void Bind() const;
        void Unbind() const;
        uint32_t GetProgram() const { return m_Program; }

    private:
        uint32_t m_Program;
        bool CompileShader(const char* source, uint32_t type, uint32_t& shader);
    };
}