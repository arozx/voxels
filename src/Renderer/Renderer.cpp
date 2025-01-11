#include "../pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "VertexArray.h"
#include "../Shader/Shader.h"
#include "../Camera/OrthographicCamera.h"

namespace Engine {
    Renderer::Renderer() {}
    Renderer::~Renderer() {}

    void Renderer::Init() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            return;
        }
        m_Camera = std::make_shared<OrthographicCamera>(-1.6f, 1.6f, -0.9f, 0.9f);
        m_PerspectiveCamera = std::make_shared<PerspectiveCamera>(45.0f, 1280.0f/720.0f);
    }

    void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray,
        const std::shared_ptr<Shader>& shader,
        GLenum primitiveType) 
        {
        Submit(vertexArray, shader, Transform(), primitiveType);
    }

    void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray,
        const std::shared_ptr<Shader>& shader,
        const Transform& transform,
        GLenum primitiveType) 
        {
        shader->Bind();
        if (m_CameraType == CameraType::Orthographic) {
            shader->SetMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
        } else {
            shader->SetMat4("u_ViewProjection", m_PerspectiveCamera->GetViewProjectionMatrix());
        }
        shader->SetMat4("u_Model", transform.GetModelMatrix());
        
        RenderCommand command;
        command.vertexArray = vertexArray;
        command.shader = shader;
        command.primitiveType = primitiveType;
        command.transform = transform;
        m_CommandQueue.push(command);
    }

    void Renderer::Flush() {
        while (!m_CommandQueue.empty()) {
            const auto& command = m_CommandQueue.front();
            command.shader->Bind();
            command.shader->SetMat4("u_Model", command.transform.GetModelMatrix());
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
        Flush();
    }

    void Renderer::SetCameraType(CameraType type) {
        m_CameraType = type;
    }
}