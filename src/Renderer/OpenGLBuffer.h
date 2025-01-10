#pragma once

#include "Buffer.h"

namespace Engine {
    
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(float* vertices, uint32_t size);
        virtual ~OpenGLVertexBuffer();

        virtual void SetData(float* vertices, uint32_t size) override;
        
        virtual void Bind() const override;
        virtual void UnBind() const override;

    private:
        uint32_t m_RendererID;
    };

    class OpenGLIndexBuffer : public IndexBuffer
    {
    public: 
        OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
        virtual ~OpenGLIndexBuffer();

        virtual void Bind() const override;
        virtual void UnBind() const override;

    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };
}