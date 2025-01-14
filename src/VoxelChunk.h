#pragma once

#include "VoidNoise.h"
#include <pch.h>

/**
 * @brief Represents a cubic chunk of voxels
 * @details Manages storage and generation of voxels within a fixed-size chunk
 */
class VoxelChunk {
public:
    /** @brief Size of chunk in each dimension */
    static constexpr int CHUNK_SIZE = 32;
    
    /**
     * @brief Construct a new Voxel Chunk
     * @param chunkX X coordinate of chunk in chunk space
     * @param chunkY Y coordinate of chunk in chunk space
     * @param chunkZ Z coordinate of chunk in chunk space
     */
    VoxelChunk(int chunkX, int chunkY, int chunkZ);

    /**
     * @brief Generate terrain within the chunk
     * @param noiseGenerator Noise generator for terrain generation
     * @param scale Scale factor for noise generation
     */
    void generate(const VoidNoise& noiseGenerator, float scale);

    /**
     * @brief Get raw voxel data array
     * @return const std::array<bool, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>& Reference to voxel data
     */
    const std::array<bool, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>& getData() const { return m_VoxelData; }
    
    /**
     * @brief Get chunk position in chunk space
     * @return glm::ivec3 Position vector
     */
    glm::ivec3 getPosition() const { return glm::ivec3(m_ChunkX, m_ChunkY, m_ChunkZ); }

private:
    std::array<bool, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> m_VoxelData;
    int m_ChunkX, m_ChunkY, m_ChunkZ;
    
    /**
     * @brief Convert 3D coordinates to array index
     * @param x X coordinate within chunk
     * @param y Y coordinate within chunk
     * @param z Z coordinate within chunk
     * @return int Array index for coordinates
     */
    int getIndex(int x, int y, int z) const;
};