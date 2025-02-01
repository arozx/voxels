
#pragma once
#include <pch.h>

#include "Camera/OrthographicCamera.h"
#include "Material.h"
#include "VertexArray.h"

namespace Engine {

struct BatchVertex {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    float TexIndex;
    float TilingFactor;
};

class BatchRenderer2D {
   public:
    static const uint32_t MaxQuads = 100000;
    static const uint32_t MaxVertices = MaxQuads * 4;
    static const uint32_t MaxIndices = MaxQuads * 6;
    static const uint32_t MaxTextureSlots = 32;

    BatchRenderer2D();
    ~BatchRenderer2D();

    void Initialize();
    void Shutdown();

    void BeginScene(const std::shared_ptr<OrthographicCamera>& camera);
    void EndScene();

    void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    void DrawQuad(const glm::vec3& position, const glm::vec2& size,
                  const std::shared_ptr<Texture>& texture, float tilingFactor = 1.0f,
                  const glm::vec4& tintColor = glm::vec4(1.0f));

   private:
    void StartBatch();
    void NextBatch();
    void Flush();

    std::shared_ptr<VertexArray> m_QuadVA;
    std::shared_ptr<VertexBuffer> m_QuadVB;
    std::shared_ptr<Material> m_Material;
    std::shared_ptr<Texture> m_WhiteTexture;

    uint32_t m_QuadIndexCount = 0;
    BatchVertex* m_VertexBufferBase = nullptr;
    BatchVertex* m_VertexBufferPtr = nullptr;

    std::array<std::shared_ptr<Texture>, MaxTextureSlots> m_TextureSlots;
    uint32_t m_TextureSlotIndex = 1;

    glm::vec4 m_QuadVertexPositions[4];

    struct Statistics {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;

        void Reset() {
            DrawCalls = 0;
            QuadCount = 0;
        }
    } m_Stats;
};

}  // namespace Engine