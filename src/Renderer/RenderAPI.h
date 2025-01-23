#pragma once
#include <pch.h>

namespace Engine {
    class RenderAPI {
    public:
        enum class API {
            None = 0,
            OpenGL,
            Vulkan,
            DirectX11,
            DirectX12
        };

        virtual ~RenderAPI() = default;
        
        virtual void Init() = 0;
        virtual void SetViewport(int x, int y, int width, int height) = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;
        
        virtual void DrawIndexed(uint32_t indexCount) = 0;
        virtual void DrawArrays(uint32_t vertexCount) = 0;
        
        static API GetAPI() { return s_API; }
        static void SetAPI(API api) { s_API = api; }
        
        static RenderAPI* Create();

    private:
        static API s_API;
    };
}