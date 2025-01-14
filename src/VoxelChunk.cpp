#include "VoxelChunk.h"

/**
 * @brief Initialize chunk with empty voxels
 * @param chunkX X coordinate in chunk space
 * @param chunkY Y coordinate in chunk space
 * @param chunkZ Z coordinate in chunk space
 * @details Voxel data is initialized to false (air)
 */

VoxelChunk::VoxelChunk(int chunkX, int chunkY, int chunkZ)
    : m_ChunkX(chunkX), m_ChunkY(chunkY), m_ChunkZ(chunkZ) {
    m_VoxelData.fill(false);
}

/**
 * @brief Generate terrain data for the chunk
 * @param noiseGenerator Noise generator instance
 * @param scale Scale factor for noise generation
 * @details Uses multiple noise octaves for terrain height and cave generation
 */
void VoxelChunk::generate(const VoidNoise& noiseGenerator, float scale) {
    // Ground level (adjust this to change the base height of the terrain)
    const int BASE_HEIGHT = 32;
    const float NOISE_SCALE = scale * 0.05f;
    const float HEIGHT_SCALE = 32.0f;         // Maximum height variation

    // Calculate world position of chunk origin
    float worldX = m_ChunkX * CHUNK_SIZE;
    float worldZ = m_ChunkZ * CHUNK_SIZE;

    // Generate terrain for this chunk
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            // Sample heightmap at world position
            float wx = (worldX + x) * NOISE_SCALE;
            float wz = (worldZ + z) * NOISE_SCALE;
            
            // Generate height using multiple noise octaves for more natural terrain
            float height = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;
            
            for (int octave = 0; octave < 4; octave++) {
                height += noiseGenerator.noise(wx * frequency, wz * frequency) * amplitude;
                amplitude *= 0.5f;
                frequency *= 2.0f;
            }

            // Normalize height and scale to desired range
            height = (height + 1.0f) * 0.5f;  // Normalize to 0-1
            int terrainHeight = static_cast<int>(height * HEIGHT_SCALE);

            // Fill voxels from bottom up to height
            for (int y = 0; y < CHUNK_SIZE; y++) {
                int worldY = m_ChunkY * CHUNK_SIZE + y;
                
                // Basic terrain with height variation
                if (worldY < terrainHeight + BASE_HEIGHT) {
                    m_VoxelData[getIndex(x, y, z)] = true;
                }
            }
        }
    }

    // Add 3D noise features (caves, etc)
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (!m_VoxelData[getIndex(x, y, z)]) continue;

                float wx = (worldX + x) * NOISE_SCALE * 2.0f;
                float wy = (m_ChunkY * CHUNK_SIZE + y) * NOISE_SCALE * 2.0f;
                float wz = (worldZ + z) * NOISE_SCALE * 2.0f;
                
                // Generate 3D noise for caves
                float caveNoise = noiseGenerator.noise(wx, wy + wz);
                if (caveNoise > 0.8f) {
                    m_VoxelData[getIndex(x, y, z)] = false;
                }
            }
        }
    }
}

int VoxelChunk::getIndex(int x, int y, int z) const {
    return x + CHUNK_SIZE * (y + CHUNK_SIZE * z);
}