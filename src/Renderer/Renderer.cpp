#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine {
    Renderer::Renderer() {}

    Renderer::~Renderer() {
        glDeleteProgram(m_ShaderProgram);
    }

    void Renderer::Init() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            return;
        }

        const char* vertexShaderSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            void main() {
                gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
            }
        )";
        const char* fragmentShaderSource = R"(
            #version 330 core
            out vec4 FragColor;
            void main() {
                FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
            }
        )";

        // Vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        // Check shader compilation
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            // Handle error
        }

        // Fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            // Handle error
        }

        // Shader program
        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, vertexShader);
        glAttachShader(m_ShaderProgram, fragmentShader);
        glLinkProgram(m_ShaderProgram);

        glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(m_ShaderProgram, 512, NULL, infoLog);
            // Handle error
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Create vertex array and buffers
        glGenVertexArrays(1, &m_VertexArray);
        glBindVertexArray(m_VertexArray);

        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        uint32_t indices[] = { 0, 1, 2 };

        m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
        m_IndexBuffer.reset(IndexBuffer::Create(indices, 3));

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void Renderer::Draw() {
        glUseProgram(m_ShaderProgram);
        glBindVertexArray(m_VertexArray);
        m_IndexBuffer->Bind();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    }
}