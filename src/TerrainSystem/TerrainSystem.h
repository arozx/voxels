#pragma once

#include "Renderer/VertexArray.h"
#include "Renderer/Material.h"
#include "Renderer/Renderer.h"
#include "VoxelTerrain.h"

namespace Engine {
    class TerrainSystem {
    public:
        TerrainSystem();
        ~TerrainSystem() = default;

        void Initialize(Renderer& renderer);
        void Update(float deltaTime);
        void Render(Renderer& renderer);
        void RegenerateTerrain(uint32_t seed);

    private:
        void GenerateMesh();

        std::unique_ptr<VoxelTerrain> m_Terrain;
        std::shared_ptr<VertexArray> m_TerrainVA;
        std::shared_ptr<Shader> m_TerrainShader;
        std::shared_ptr<Material> m_TerrainMaterial;
        std::shared_ptr<Texture> m_TerrainTexture;
        Transform m_TerrainTransform;
    };
}