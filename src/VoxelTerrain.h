#pragma once

#include "VoxelChunk.h"
#include "VoidNoise.h"
#include <pch.h>

/**
 * @brief Manages the voxel-based terrain system
 * @details Handles chunk generation, storage, and voxel access across the world
 */
class VoxelTerrain {
public:
    /**
     * @brief Construct a new Voxel Terrain
     * @param seed Random seed for terrain generation
     */
    VoxelTerrain(unsigned int seed = 1234);

    /**
     * @brief Generate a new chunk at specified coordinates
     * @param chunkX X coordinate of chunk
     * @param chunkY Y coordinate of chunk
     * @param chunkZ Z coordinate of chunk
     */
    void generateChunk(int chunkX, int chunkY, int chunkZ);

    /**
     * @brief Get voxel state at specified world coordinates
     * @param x X coordinate in world space
     * @param y Y coordinate in world space
     * @param z Z coordinate in world space
     * @return bool True if voxel is solid, false otherwise
     */
    bool getVoxel(int x, int y, int z) const;

    /**
     * @brief Set voxel state at specified world coordinates
     * @param x X coordinate in world space
     * @param y Y coordinate in world space
     * @param z Z coordinate in world space
     * @param value New voxel state
     */
    void setVoxel(int x, int y, int z, bool value);

    void setTerrainParameters(float noiseScale, float terrainScale, int waterLevel, int maxHeight) {
        m_NoiseScale = noiseScale;
        m_TerrainScale = terrainScale;
        m_WaterLevel = waterLevel;
        m_MaxHeight = maxHeight;
    }

private:
    VoidNoise m_NoiseGenerator;
    std::unordered_map<uint64_t, std::unique_ptr<VoxelChunk>> m_Chunks;
    float m_TerrainScale = 8.0f;
    float m_NoiseScale = 3.0f;
    int m_WaterLevel = 32;
    int m_MaxHeight = 128;
    
    /**
     * @brief Generate unique key for chunk coordinates
     * @param x Chunk X coordinate
     * @param y Chunk Y coordinate
     * @param z Chunk Z coordinate
     * @return uint64_t Unique chunk identifier
     */
    static uint64_t getChunkKey(int x, int y, int z);
    
    /**
     * @brief Get chunk at specified coordinates
     * @param chunkX Chunk X coordinate
     * @param chunkY Chunk Y coordinate
     * @param chunkZ Chunk Z coordinate
     * @return VoxelChunk* Pointer to chunk or nullptr if not found
     */
    VoxelChunk* getChunk(int chunkX, int chunkY, int chunkZ) const;

    void SaveHeightmapDebug(int chunkX, int chunkZ);
};