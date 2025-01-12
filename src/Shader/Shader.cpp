#include "Shader.h"
#include <glad/glad.h>
#include "pch.h"
#include "../Logging.h"

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

    void Shader::SetMat4(const std::string& name, const glm::mat4& matrix) {
        GLint location = glGetUniformLocation(m_Program, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::SetInt(const std::string& name, int value) {
        GLint location = glGetUniformLocation(m_Program, name.c_str());
        glUniform1i(location, value);
    }

    void Shader::SetFloat(const std::string& name, float value) {
        GLint location = glGetUniformLocation(m_Program, name.c_str());
        glUniform1f(location, value);
    }

    void Shader::SetVector2(const std::string& name, const glm::vec2& value) {
        GLint location = glGetUniformLocation(m_Program, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void Shader::SetVector3(const std::string& name, const glm::vec3& value) {
        GLint location = glGetUniformLocation(m_Program, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void Shader::SetVector4(const std::string& name, const glm::vec4& value) {
        GLint location = glGetUniformLocation(m_Program, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    Shader* Shader::CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string vertexSrc = ReadFile(vertexPath);
        std::string fragmentSrc = ReadFile(fragmentPath);
        
        if (vertexSrc.empty() || fragmentSrc.empty()) {
            return nullptr;
        }
        
        return new Shader(vertexSrc.c_str(), fragmentSrc.c_str());
    }

    std::string Shader::ReadFile(const std::string& filepath) {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in) {
            in.seekg(0, std::ios::end);
            result.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&result[0], result.size());
            in.close();
        } else {
            LOG_ERROR("Could not open file: {0}", filepath);
            return "";
        }
        return result;
    }
}