#include "../pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "VertexArray.h"
#include "../Shader/Shader.h"

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

        m_Shader = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);

        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
        };

        uint32_t indices[] = { 0, 1, 2 };

        m_VertexArray.reset(VertexArray::Create());
        
        std::shared_ptr<VertexBuffer> vertexBuffer(VertexBuffer::Create(vertices, sizeof(vertices)));
        
        BufferLayout layout = {
            { ShaderDataType::Float3, "aPosition" }
        };
        
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        std::shared_ptr<IndexBuffer> indexBuffer(IndexBuffer::Create(indices, 3));
        m_VertexArray->SetIndexBuffer(indexBuffer);
    }

    void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray,
        const std::shared_ptr<Shader>& shader,
        GLenum primitiveType) 
        {
        RenderCommand command;
        command.vertexArray = vertexArray;
        command.shader = shader;
        command.primitiveType = primitiveType;
        m_CommandQueue.push(command);
    }

    void Renderer::Flush() {
        while (!m_CommandQueue.empty()) {
            const auto& command = m_CommandQueue.front();
            command.shader->Bind();
            command.vertexArray->Bind();
            glDrawElements(command.primitiveType, 
                command.vertexArray->GetIndexBuffer()->GetCount(), 
                GL_UNSIGNED_INT, 
                nullptr);
            command.vertexArray->Unbind();
            command.shader->Unbind();
            m_CommandQueue.pop();
        }
    }

    void Renderer::Draw() {
        // Submit the default triangle
        Submit(m_VertexArray, m_Shader, GL_TRIANGLES);
        // Execute all queued commands
        Flush();
    }
}