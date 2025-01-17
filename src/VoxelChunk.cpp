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
    const float NOISE_SCALE = scale * 0.01f;
    const int WATER_LEVEL = 32;
    const float PEAK_FACTOR = 2.0f;  // Controls how pointy the peaks are
    
    // World position of chunk origin
    float worldX = m_ChunkX * CHUNK_SIZE;
    float worldZ = m_ChunkZ * CHUNK_SIZE;
    int chunkYStart = m_ChunkY * CHUNK_SIZE;

    // Generate heightmap first
    std::array<int, CHUNK_SIZE * CHUNK_SIZE> heightMap;
    
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            float wx = (worldX + x) * NOISE_SCALE;
            float wz = (worldZ + z) * NOISE_SCALE;
            
            // Generate terrain height with modified weights
            float continentNoise = noiseGenerator.noise(wx * 0.5f, wz * 0.5f) * 2.0f - 1.0f;
            float terrainNoise = (noiseGenerator.noise(wx * 2.0f, wz * 2.0f) * 2.0f - 1.0f) * 0.7f;
            float detailNoise = (noiseGenerator.noise(wx * 4.0f, wz * 4.0f) * 2.0f - 1.0f) * 0.3f;
            
            float combinedNoise = continentNoise + terrainNoise + detailNoise;
            
            // Apply exponential function to create peaks
            float normalizedHeight = (combinedNoise + 1.0f) * 0.5f;
            float peakHeight = std::pow(normalizedHeight, PEAK_FACTOR);
            
            // Convert to final height
            int height = static_cast<int>(peakHeight * 64.0f) + WATER_LEVEL;
            
            heightMap[x + z * CHUNK_SIZE] = height;
        }
    }
    
    // Fill voxels and block types based on height
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int terrainHeight = heightMap[x + z * CHUNK_SIZE];
            
            for (int y = 0; y < CHUNK_SIZE; y++) {
                int worldY = chunkYStart + y;
                int index = getIndex(x, y, z);
                
                if (worldY < terrainHeight) {
                    m_VoxelData[index] = true;
                    m_BlockTypes[index] = getBlockType(worldY, terrainHeight);
                } else {
                    m_VoxelData[index] = false;
                    m_BlockTypes[index] = BlockType::Air;
                }
            }
        }
    }
}

BlockType VoxelChunk::getBlockType(int worldY, int height) const {
    if (worldY >= height) return BlockType::Air;
    
    const int SNOW_HEIGHT = 90;
    const int STONE_HEIGHT = 40;
    const int DIRT_DEPTH = 5;
    
    if (height >= SNOW_HEIGHT) return BlockType::Snow;
    if (worldY == height - 1) return BlockType::Grass;
    if (worldY > height - DIRT_DEPTH) return BlockType::Dirt;
    if (worldY <= STONE_HEIGHT) return BlockType::Stone;
    return BlockType::Stone;
}

int VoxelChunk::getIndex(int x, int y, int z) const {
    return x + CHUNK_SIZE * (y + CHUNK_SIZE * z);
}