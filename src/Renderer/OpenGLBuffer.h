#pragma once

#include "Buffer.h"
#include <glad/glad.h>

namespace Engine {
    /**
     * @brief OpenGL implementation of vertex buffer
     */
    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        /**
         * @brief Creates an OpenGL vertex buffer
         * @param vertices Pointer to vertex data
         * @param size Size of vertex data in bytes
         */
        OpenGLVertexBuffer(const float* vertices, uint32_t size);
        virtual ~OpenGLVertexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
        virtual const BufferLayout& GetLayout() const override { return m_Layout; }

    private:
        uint32_t m_RendererID;     ///< OpenGL buffer ID
        BufferLayout m_Layout;      ///< Buffer layout description
    };

    /**
     * @brief OpenGL implementation of index buffer
     */
    class OpenGLIndexBuffer : public IndexBuffer {
    public:
        /**
         * @brief Creates an OpenGL index buffer
         * @param indices Pointer to index data
         * @param count Number of indices
         */
        OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32_t GetCount() const override { return m_Count; }
    private:
        uint32_t m_RendererID;     ///< OpenGL buffer ID
        uint32_t m_Count;          ///< Number of indices
    };
}