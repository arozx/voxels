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
        void SetChunkRange(int range) { m_ChunkRange = range; GenerateMesh(); }
        int GetChunkRange() const { return m_ChunkRange; }

        void SetBaseHeight(float height);
        void SetHeightScale(float scale);
        void SetNoiseScale(float scale);
        
        float GetBaseHeight() const { return m_BaseHeight; }
        float GetHeightScale() const { return m_HeightScale; }
        float GetNoiseScale() const { return m_NoiseScale; }

    private:
        void GenerateMesh();

        std::unique_ptr<VoxelTerrain> m_Terrain;
        std::shared_ptr<VertexArray> m_TerrainVA;
        std::shared_ptr<Shader> m_TerrainShader;
        std::shared_ptr<Material> m_TerrainMaterial;
        std::shared_ptr<Texture> m_TerrainTexture;
        Transform m_TerrainTransform;
        int m_ChunkRange = 1; // Controls how many chunks to generate in each direction

        float m_BaseHeight = 32.0f;
        float m_HeightScale = 32.0f;
        float m_NoiseScale = 0.05f;
    };
}