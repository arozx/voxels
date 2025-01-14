#pragma once
#include "VertexArray.h"

namespace Engine {
    /**
     * @brief OpenGL implementation of vertex array object
     * 
     * Manages vertex attribute configuration and buffer binding for OpenGL.
     */
    class OpenGLVertexArray : public VertexArray {
    public:
        /** @brief Creates a new OpenGL vertex array object */
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        /** @brief Binds this vertex array for rendering */
        virtual void Bind() const override;
        /** @brief Unbinds this vertex array */
        virtual void Unbind() const override;

        /**
         * @brief Adds a vertex buffer with attribute layout
         * @param vertexBuffer Buffer containing vertex data
         */
        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;

        /**
         * @brief Sets the index buffer for indexed rendering
         * @param indexBuffer Buffer containing index data
         */
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

        /** @return List of attached vertex buffers */
        virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
        /** @return Current index buffer */
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

    private:
        uint32_t m_RendererID;           ///< OpenGL vertex array object ID
        uint32_t m_VertexBufferIndex = 0;    ///< Current attribute index
        std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;  ///< Attached vertex buffers
        std::shared_ptr<IndexBuffer> m_IndexBuffer;                  ///< Current index buffer
    };
}