
#include "VoxelTerrain.h"

VoxelTerrain::VoxelTerrain(unsigned int seed) : m_NoiseGenerator(seed) {
    generate();
}

void VoxelTerrain::generate() {
    // Generate 2D heightmap
    float scale = 4.0f; // Adjust this to change terrain frequency
    auto heightmap = m_NoiseGenerator.generateHeightmap(TERRAIN_SIZE, TERRAIN_SIZE, scale);

    // Clear voxel data
    m_VoxelData.fill(false);

    // Generate terrain from heightmap
    for (int x = 0; x < TERRAIN_SIZE; x++) {
        for (int z = 0; z < TERRAIN_SIZE; z++) {
            // Get height value from heightmap (normalized 0-1)
            float heightValue = heightmap[z * TERRAIN_SIZE + x];
            
            // Convert to voxel height (0 to TERRAIN_SIZE)
            int height = static_cast<int>(heightValue * TERRAIN_SIZE);
            
            // Fill voxels from bottom up to height
            for (int y = 0; y < height; y++) {
                m_VoxelData[getIndex(x, y, z)] = true;
            }
        }
    }

    // Add some noise to the terrain using 3D sampling
    for (int x = 0; x < TERRAIN_SIZE; x++) {
        for (int y = 0; y < TERRAIN_SIZE; y++) {
            for (int z = 0; z < TERRAIN_SIZE; z++) {
                // Sample 3D noise
                float nx = static_cast<float>(x) * scale / TERRAIN_SIZE;
                float ny = static_cast<float>(y) * scale / TERRAIN_SIZE;
                float nz = static_cast<float>(z) * scale / TERRAIN_SIZE;
                
                // Use 2D noise for each layer
                float caveNoise = m_NoiseGenerator.noise(nx + nz, ny);
                
                // Create caves where noise value is high
                if (caveNoise > 0.75f && m_VoxelData[getIndex(x, y, z)]) {
                    m_VoxelData[getIndex(x, y, z)] = false;
                }
            }
        }
    }
}