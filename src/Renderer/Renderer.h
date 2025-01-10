#pragma once
#include "Buffer.h"
#include <cstdint>
#include <memory>

namespace Engine {
    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void Init();
        void Draw();

    private:
        uint32_t m_ShaderProgram;
        uint32_t m_VertexArray;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
    };
}