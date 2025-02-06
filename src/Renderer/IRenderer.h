#pragma once
#include <memory>

namespace Engine {
class IRenderer {
   public:
    virtual ~IRenderer() = default;

    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual void BeginScene() = 0;
    virtual void EndScene() = 0;
    virtual void Flush() = 0;

    struct Statistics {
        uint32_t DrawCalls = 0;
        uint32_t VertexCount = 0;
        uint32_t IndexCount = 0;

        void Reset() { memset(this, 0, sizeof(Statistics)); }
    };

    virtual Statistics GetStats() const = 0;
    virtual void ResetStats() = 0;
};
}  // namespace Engine
