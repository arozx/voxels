#pragma once

#include "VoidNoise.h"
#include <pch.h>

class VoxelChunk {
public:
    static constexpr int CHUNK_SIZE = 32;
    
    VoxelChunk(int chunkX, int chunkY, int chunkZ);
    void generate(const VoidNoise& noiseGenerator, float scale);
    const std::array<bool, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>& getData() const { return m_VoxelData; }
    
    glm::ivec3 getPosition() const { return glm::ivec3(m_ChunkX, m_ChunkY, m_ChunkZ); }

private:
    std::array<bool, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> m_VoxelData;
    int m_ChunkX, m_ChunkY, m_ChunkZ;
    
    int getIndex(int x, int y, int z) const {
        return x + CHUNK_SIZE * (y + CHUNK_SIZE * z);
    }
};