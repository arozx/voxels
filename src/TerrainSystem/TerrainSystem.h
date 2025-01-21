#pragma once

#include <pch.h>
#include "Renderer/Renderer.h"
#include "Noise/NoiseGenerator.h"
#include "Noise/VoidNoise/VoidNoise.h"
#include "Noise/PerlinNoise/PerlinNoise.h"
#include "Noise/SimplexNoise/SimplexNoise.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Material.h"
#include "VoxelTerrain.h"
#include "BlockTypes.h"

namespace Engine {
    /**
     * @brief Manages voxel terrain generation and rendering
     * 
     * Handles terrain mesh generation, updates, and rendering using
     * a voxel-based system with configurable parameters.
     */
    class TerrainSystem {
    public:
        TerrainSystem();
        ~TerrainSystem() = default;

        /** @brief Initializes terrain system with renderer */
        void Initialize(Renderer& renderer);
        
        /**
         * @brief Updates terrain state
         * @param deltaTime Time since last update
         */
        void Update(float deltaTime);
        
        /**
         * @brief Renders terrain using provided renderer
         * @param renderer Renderer to use
         */
        void Render(Renderer& renderer);
        
        /**
         * @brief Regenerates terrain with new seed
         * @param seed Random seed for terrain generation
         */
        void RegenerateTerrain(uint32_t seed);

        /**
         * @brief Sets the range of chunks to generate
         * @param range Number of chunks in each direction
         */
        void SetChunkRange(int range) { m_ChunkRange = range; GenerateMesh(); }
        
        /** @return Current chunk generation range */
        int GetChunkRange() const { return m_ChunkRange; }

        /** @brief Sets base height for terrain generation */
        void SetBaseHeight(float height);
        /** @brief Sets height scale factor for terrain */
        void SetHeightScale(float scale);
        /** @brief Sets noise scale for terrain variation */
        void SetNoiseScale(float scale);
        
        /** @return Base terrain height */
        float GetBaseHeight() const { return m_BaseHeight; }
        /** @return Height scale factor */
        float GetHeightScale() const { return m_HeightScale; }
        /** @return Noise scale factor */
        float GetNoiseScale() const { return m_NoiseScale; }

    private:
        /** @brief Generates terrain mesh based on current parameters */
        void GenerateMesh();

        std::unique_ptr<VoxelTerrain> m_Terrain;        ///< Voxel data container
        std::shared_ptr<VertexArray> m_TerrainVA;       ///< Terrain vertex array
        std::shared_ptr<Shader> m_TerrainShader;        ///< Terrain shader
        std::shared_ptr<Material> m_TerrainMaterial;    ///< Terrain material
        std::shared_ptr<Texture> m_TerrainTexture;      ///< Terrain texture
        Transform m_TerrainTransform;                   ///< Terrain transformation
        int m_ChunkRange = 1;                          ///< Chunk generation range

        float m_BaseHeight = 32.0f;    ///< Base terrain height
        float m_HeightScale = 32.0f;   ///< Height variation scale
        float m_NoiseScale = 0.05f;    ///< Noise variation scale

        // Noise generator
        NoiseGenerator<VoidNoise> m_NoiseGen;
    };
}