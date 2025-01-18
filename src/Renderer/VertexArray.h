#pragma once

#include "Buffer.h"

namespace Engine {
    /**
     * @brief Abstract base class for vertex array objects
     * 
     * Represents a vertex array object that stores vertex attributes
     * and their configurations for rendering.
     */
    class VertexArray {
    public:
        /**
         * @brief Creates a new platform-specific vertex array
         * @return Pointer to the created vertex array
         */
        static VertexArray* Create();
        
        virtual ~VertexArray() = default;

        /**
         * @brief Bind the vertex array
         */
        virtual void Bind() const = 0;

        /**
         * @brief Unbind the vertex array
         */
        virtual void Unbind() const = 0;

        /**
         * @brief Add a vertex buffer to the array
         * @param vertexBuffer Buffer containing vertex data
         */
        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;

        /**
         * @brief Set the index buffer for this array
         * @param indexBuffer Buffer containing index data
         */
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

        /**
         * @brief Get all vertex buffers
         * @return Vector of vertex buffer references
         */
        virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;

        /**
         * @brief Get the index buffer
         * @return Reference to index buffer
         */
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;
    };
}