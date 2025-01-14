#pragma once

#include "VoxelChunk.h"
#include "VoidNoise.h"
#include <pch.h>


class VoxelTerrain {
public:
    VoxelTerrain(unsigned int seed = 1234);
    void generateChunk(int chunkX, int chunkY, int chunkZ);
    bool getVoxel(int x, int y, int z) const;
    void setVoxel(int x, int y, int z, bool value);

private:
    VoidNoise m_NoiseGenerator;
    std::unordered_map<uint64_t, std::unique_ptr<VoxelChunk>> m_Chunks;
    float m_TerrainScale = 4.0f;
    
    static uint64_t getChunkKey(int x, int y, int z) {
        return ((uint64_t)x & 0xFFFFF) | 
            (((uint64_t)y & 0xFFFFF) << 20) |
            (((uint64_t)z & 0xFFFFF) << 40);
    }
    
    VoxelChunk* getChunk(int chunkX, int chunkY, int chunkZ) const;
};