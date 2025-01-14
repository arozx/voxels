/**
 * @file OpenGLVertexArray.cpp
 * @brief Implementation of OpenGL vertex array object functionality
 */
#include "OpenGLVertexArray.h"
#include <glad/glad.h>

namespace Engine {
    /**
     * @brief Converts engine shader data types to OpenGL types
     * @param type Engine shader data type
     * @return Corresponding OpenGL data type
     */
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:  return GL_FLOAT;
            case ShaderDataType::Float2: return GL_FLOAT;
            case ShaderDataType::Float3: return GL_FLOAT;
            case ShaderDataType::Float4: return GL_FLOAT;
            case ShaderDataType::Mat3:   return GL_FLOAT;
            case ShaderDataType::Mat4:   return GL_FLOAT;
            case ShaderDataType::Int:    return GL_INT;
            case ShaderDataType::Int2:   return GL_INT;
            case ShaderDataType::Int3:   return GL_INT;
            case ShaderDataType::Int4:   return GL_INT;
            case ShaderDataType::Bool:   return GL_BOOL;
            default: return 0;
        }
    }

    /**
     * @brief Constructs and initializes an OpenGL vertex array object
     */
    OpenGLVertexArray::OpenGLVertexArray() {
        glGenVertexArrays(1, &m_RendererID);
    }

    /**
     * @brief Cleans up the OpenGL vertex array object
     */
    OpenGLVertexArray::~OpenGLVertexArray() {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    /**
     * @brief Binds this vertex array for rendering
     */
    void OpenGLVertexArray::Bind() const {
        glBindVertexArray(m_RendererID);
    }

    /**
     * @brief Unbinds this vertex array
     */
    void OpenGLVertexArray::Unbind() const {
        glBindVertexArray(0);
    }

    /**
     * @brief Adds a vertex buffer and configures its attributes
     * @param vertexBuffer Buffer containing vertex data
     * 
     * Sets up vertex attribute pointers based on the buffer's layout
     */
    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
        glBindVertexArray(m_RendererID);
        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribPointer(
                m_VertexBufferIndex,
                GetComponentCount(element.Type),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)(intptr_t)element.Offset
            );
            m_VertexBufferIndex++;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    /**
     * @brief Sets the index buffer for indexed rendering
     * @param indexBuffer Buffer containing index data
     */
    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
        glBindVertexArray(m_RendererID);
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }
}