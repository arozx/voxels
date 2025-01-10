#pragma once
#include "Buffer.h"
#include "../Shader/Shader.h"
#include <queue>
#include <glad/glad.h>

namespace Engine {
    class Shader;
    class VertexArray;

    struct RenderCommand {
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<Shader> shader;
        GLenum primitiveType;
    };

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void Init();
        void Draw();
        void Submit(const std::shared_ptr<VertexArray>& vertexArray, 
            const std::shared_ptr<Shader>& shader,
            GLenum primitiveType = GL_TRIANGLES);
        void Flush();

    private:
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;
        std::queue<RenderCommand> m_CommandQueue;
    };
}