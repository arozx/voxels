/**
 * @file Shader.cpp
 * @brief Implementation of OpenGL shader program management
 * 
 * Handles shader compilation, linking, and uniform management
 * for OpenGL shader programs.
 */
#include <pch.h>
#include "Shader.h"
#include "../Core/AssetManager.h"
#include "ShaderHotReload.h"
#include <glad/glad.h>

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

    bool Shader::LoadFromSource(const char* vertexSrc, const char* fragmentSrc) {
        uint32_t vertexShader = 0, fragmentShader = 0;
        
        if (!CompileShader(vertexSrc, GL_VERTEX_SHADER, vertexShader) ||
            !CompileShader(fragmentSrc, GL_FRAGMENT_SHADER, fragmentShader)) {
            return false;
        }

        m_Program = glCreateProgram();
        glAttachShader(m_Program, vertexShader);
        glAttachShader(m_Program, fragmentShader);
        glLinkProgram(m_Program);

        int success;
        char infoLog[512];
        glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(m_Program, 512, NULL, infoLog);
            std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
            return false;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        m_IsLoaded = true;
        return true;
    }

    bool Shader::Load(const std::string& path) {
        size_t separator = path.find(';');
        if (separator == std::string::npos) {
            LOG_ERROR("Invalid shader path format: {}", path);
            return false;
        }

        std::string vertexPath = path.substr(0, separator);
        std::string fragmentPath = path.substr(separator + 1);
        
        std::string vertexSrc = ReadFile(vertexPath);
        std::string fragmentSrc = ReadFile(fragmentPath);
        
        if (vertexSrc.empty() || fragmentSrc.empty()) {
            return false;
        }

        return LoadFromSource(vertexSrc.c_str(), fragmentSrc.c_str());
    }

    std::shared_ptr<Shader> Shader::CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string combinedPath = vertexPath + ";" + fragmentPath;
        auto shader = AssetManager::Get().LoadResource<Shader>(combinedPath);
        
        #ifdef ENGINE_DEBUG
        if (shader) {
            shader->EnableHotReload(vertexPath, fragmentPath);
        }
        #endif

        return shader;
    }

    std::shared_ptr<Shader> Shader::CreateFromSource(const char* vertexSrc, const char* fragmentSrc) {
        auto shader = std::shared_ptr<Shader>(new Shader());  // Use new instead of make_shared
        if (!shader->LoadFromSource(vertexSrc, fragmentSrc)) {
            return nullptr;
        }
        return shader;
    }

    bool Shader::Reload(const std::string& path) {
        // Store current program ID
        GLuint oldProgram = m_Program;
        
        // Try to load new shader
        if (!Load(path)) {
            return false;
        }

        // Delete old program only after successful reload
        if (oldProgram) {
            glDeleteProgram(oldProgram);
        }

        return true;
    }

    void Shader::EnableHotReload(const std::string& vertPath, const std::string& fragPath) {
        ShaderHotReload::Get().WatchShader(
            std::dynamic_pointer_cast<Shader>(shared_from_this()),
            vertPath, 
            fragPath
        );
    }
}