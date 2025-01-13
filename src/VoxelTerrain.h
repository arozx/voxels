#pragma once
#include "VoidNoise.h"
#include <pch.h>

class VoxelTerrain {
public:
    static constexpr int TERRAIN_SIZE = 64;
    
    VoxelTerrain(unsigned int seed = 1234);
    void generate();
    const std::array<bool, TERRAIN_SIZE * TERRAIN_SIZE * TERRAIN_SIZE>& getData() const { return m_VoxelData; }

private:
    VoidNoise m_NoiseGenerator;
    std::array<bool, TERRAIN_SIZE * TERRAIN_SIZE * TERRAIN_SIZE> m_VoxelData;
    
    int getIndex(int x, int y, int z) const {
        return x + TERRAIN_SIZE * (y + TERRAIN_SIZE * z);
    }
};