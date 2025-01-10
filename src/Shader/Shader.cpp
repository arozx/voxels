#include "Shader.h"
#include <glad/glad.h>
#include "pch.h"
namespace Engine {
    Shader::Shader(const char* vertexSrc, const char* fragmentSrc) {
        uint32_t vertexShader = 0, fragmentShader = 0;
        
        // Compile shaders
        if (!CompileShader(vertexSrc, GL_VERTEX_SHADER, vertexShader) ||
            !CompileShader(fragmentSrc, GL_FRAGMENT_SHADER, fragmentShader)) {
            return;
        }

        // Create program
        m_Program = glCreateProgram();
        glAttachShader(m_Program, vertexShader);
        glAttachShader(m_Program, fragmentShader);
        glLinkProgram(m_Program);

        // Check linking
        int success;
        char infoLog[512];
        glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(m_Program, 512, NULL, infoLog);
            std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    Shader::~Shader() {
        glDeleteProgram(m_Program);
    }

    void Shader::Bind() const {
        glUseProgram(m_Program);
    }

    void Shader::Unbind() const {
        glUseProgram(0);
    }

    bool Shader::CompileShader(const char* source, uint32_t type, uint32_t& shader) {
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "Shader compilation failed:\n" << infoLog << std::endl;
            return false;
        }
        return true;
    }
}