#include "Renderer.h"
#include "../Shader/Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine {
    Renderer::Renderer() {}

    Renderer::~Renderer() {}

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

        m_Shader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);

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
        m_Shader->Bind();
        glBindVertexArray(m_VertexArray);
        m_IndexBuffer->Bind();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        m_Shader->Unbind();
    }
}