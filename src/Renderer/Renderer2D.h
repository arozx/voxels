#pragma once
#include <pch.h>

#include "Camera/OrthographicCamera.h"
#include "IRenderer.h"
#include "RenderObject.h"

namespace Engine {

/**
 * @brief Specialized renderer for 2D graphics with batching support
 */
class Renderer2D : public IRenderer {
   public:
    static Renderer2D& Get() {
        static Renderer2D instance;
        return instance;
    }

    // IRenderer interface implementation
    void Initialize() override;
    void Shutdown() override;
    void BeginScene() override;
    void EndScene() override;
    void Flush() override;
    Statistics GetStats() const override { return m_Stats; }
    void ResetStats() override { m_Stats.Reset(); }

    // 2D-specific methods
    void BeginScene(const std::shared_ptr<OrthographicCamera>& camera);
    void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                  const std::shared_ptr<Texture>& texture, float tilingFactor = 1.0f,
                  const glm::vec4& tintColor = glm::vec4(1.0f));
    void DrawQuad(const glm::vec3& position, const glm::vec2& size,
                  const std::shared_ptr<Texture>& texture, float tilingFactor = 1.0f,
                  const glm::vec4& tintColor = glm::vec4(1.0f));

   private:
    Renderer2D() = default;  // Private constructor
    void StartBatch();
    void NextBatch();

    Statistics m_Stats;  // Added statistics member
};
}  // namespace Engine
