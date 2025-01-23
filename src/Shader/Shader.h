#pragma once
#include <pch.h>
#include <glad/glad.h>  // Add glad include first
#include <glm/gtc/type_ptr.hpp>
#include "../Core/Resource.h"

// Forward declare
namespace Engine {
    class ShaderHotReload;
}

namespace Engine {
    /**
     * @brief OpenGL shader program wrapper
     * 
     * Manages compilation, linking and uniform setting for shader programs.
     */
    class Shader : public Resource, public std::enable_shared_from_this<Shader> {
    public:
        Shader() = default;  // Add default constructor
        /**
         * @brief Creates a shader program from source code
         * @param vertexSrc Vertex shader source code
         * @param fragmentSrc Fragment shader source code
         */
        Shader(const char* vertexSrc, const char* fragmentSrc);
        ~Shader();

        /** @brief Activates this shader program */
        void Bind() const;
        /** @brief Deactivates this shader program */
        void Unbind() const;
        /** @return OpenGL program ID */
        uint32_t GetProgram() const { return m_Program; }
        
        /**
         * @brief Sets a 4x4 matrix uniform
         * @param name Uniform name in shader
         * @param matrix Matrix value to set
         */
        void SetMat4(const std::string& name, const glm::mat4& matrix);
        /**
         * @brief Sets an integer uniform
         * @param name Uniform name in shader
         * @param value Integer value to set
         */
        void SetInt(const std::string& name, int value);
        /**
         * @brief Sets a float uniform
         * @param name Uniform name in shader
         * @param value Float value to set
         */
        void SetFloat(const std::string& name, float value);
        /**
         * @brief Sets a 2D vector uniform
         * @param name Uniform name in shader
         * @param value 2D vector value to set
         */
        void SetVector2(const std::string& name, const glm::vec2& value);
        /**
         * @brief Sets a 3D vector uniform
         * @param name Uniform name in shader
         * @param value 3D vector value to set
         */
        void SetVector3(const std::string& name, const glm::vec3& value);
        /**
         * @brief Sets a 4D vector uniform
         * @param name Uniform name in shader
         * @param value 4D vector value to set
         */
        void SetVector4(const std::string& name, const glm::vec4& value);

        /**
         * @brief Creates a shader from source files
         * @param vertexPath Path to vertex shader file
         * @param fragmentPath Path to fragment shader file
         * @return New shader instance or nullptr on failure
         */
        static std::shared_ptr<Shader> CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
        static std::shared_ptr<Shader> CreateFromSource(const char* vertexSrc, const char* fragmentSrc);

        /**
         * @brief Reads a file into a string
         * @param filepath Path to file to read
         * @return File contents as string
         */
        static std::string ReadFile(const std::string& filepath);

        // Add interface for loading from source strings
        bool LoadFromSource(const char* vertexSrc, const char* fragmentSrc);
        
        // Override Resource's Load method
        virtual bool Load(const std::string& path) override;

        /**
         * @brief Reloads the shader from its source files
         * @param path Combined vertex;fragment path
         * @return true if reload successful
         */
        bool Reload(const std::string& path);

        /**
         * @brief Enables hot-reloading for this shader
         * @param vertPath Path to vertex shader
         * @param fragPath Path to fragment shader
         */
        void EnableHotReload(const std::string& vertPath, const std::string& fragPath);

        // Implement pure virtual method
        virtual void Unload() override {
            if (m_Program) {
                glDeleteProgram(m_Program);
                m_Program = 0;
            }
            m_IsLoaded = false;
        }

    private:
        uint32_t m_Program = 0;  ///< OpenGL program ID
        /**
         * @brief Compiles a shader
         * @param source Shader source code
         * @param type Shader type (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
         * @param[out] shader Compiled shader ID
         * @return true if compilation succeeded
         */
        bool CompileShader(const char* source, uint32_t type, uint32_t& shader);
    };
}