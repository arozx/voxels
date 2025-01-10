#pragma once
#include <cstdint>

namespace Engine {
    
    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() {}

        virtual void SetData(float* vertices, uint32_t size) = 0;
        virtual void Bind() const = 0;
        virtual void UnBind() const = 0;

        static VertexBuffer* Create(float* vertices, uint32_t size);
    };

    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() {}

        virtual void Bind() const = 0;
        virtual void UnBind() const = 0;

        static IndexBuffer* Create(uint32_t* indices, uint32_t size);
    };
}