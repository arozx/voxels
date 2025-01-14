/**
 * @file Buffer.cpp
 * @brief Implementation of buffer creation factory methods
 */
#include "Buffer.h"
#include "OpenGLBuffer.h"

namespace Engine {
    /**
     * @brief Creates a vertex buffer
     * @param vertices Pointer to vertex data
     * @param size Size of vertex data in bytes
     * @return New vertex buffer instance
     */
    VertexBuffer* VertexBuffer::Create(const float* vertices, uint32_t size)
    {
        return new OpenGLVertexBuffer(vertices, size);
    }

    /**
     * @brief Creates an index buffer
     * @param indices Pointer to index data
     * @param size Size of index data in bytes
     * @return New index buffer instance
     */
    IndexBuffer* IndexBuffer::Create(const uint32_t* indices, uint32_t size)
    {
        return new OpenGLIndexBuffer(indices, size);
    }
}