
#pragma once
#include "Renderer.h"

namespace Engine {
    class OpenGLRenderer : public Renderer {
    public:
        OpenGLRenderer();
        virtual ~OpenGLRenderer();

        virtual bool Init() override;
        virtual void Draw() override;

    private:
        bool CreateShaders();

        uint32_t m_ShaderProgram;
        uint32_t m_VertexArray;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
    };
}