#include "VoxelTerrain.h"
#include "Core/Utils/BMPWriter.h"

/**
 * @brief Initialize terrain system with seed
 * @param seed Random seed for terrain generation
 */
VoxelTerrain::VoxelTerrain(unsigned int seed) : m_NoiseGenerator(seed) {
}

/**
 * @brief Generate new chunk at specified coordinates
 * @param chunkX X coordinate in chunk space
 * @param chunkY Y coordinate in chunk space
 * @param chunkZ Z coordinate in chunk space
 */
void VoxelTerrain::generateChunk(int chunkX, int chunkY, int chunkZ) {
    uint64_t key = getChunkKey(chunkX, chunkY, chunkZ);
    auto chunk = std::make_unique<VoxelChunk>(chunkX, chunkY, chunkZ);
    chunk->generate(m_NoiseGenerator, m_TerrainScale);
    
    // Debug: Save heightmap when generating chunk at y=0
    if (chunkY == 0) {
        SaveHeightmapDebug(chunkX, chunkZ);
    }
    
    m_Chunks[key] = std::move(chunk);
}

/**
 * @brief Get voxel state at world coordinates
 * @param x X coordinate in world space
 * @param y Y coordinate in world space
 * @param z Z coordinate in world space
 * @return bool True if voxel is solid
 */
bool VoxelTerrain::getVoxel(int x, int y, int z) const {
    int chunkX = x / VoxelChunk::CHUNK_SIZE;
    int chunkY = y / VoxelChunk::CHUNK_SIZE;
    int chunkZ = z / VoxelChunk::CHUNK_SIZE;
    
    VoxelChunk* chunk = getChunk(chunkX, chunkY, chunkZ);
    if (!chunk) return false;
    
    int localX = x % VoxelChunk::CHUNK_SIZE;
    int localY = y % VoxelChunk::CHUNK_SIZE;
    int localZ = z % VoxelChunk::CHUNK_SIZE;
    
    if (localX < 0) localX += VoxelChunk::CHUNK_SIZE;
    if (localY < 0) localY += VoxelChunk::CHUNK_SIZE;
    if (localZ < 0) localZ += VoxelChunk::CHUNK_SIZE;
    
    return chunk->getData()[localX + VoxelChunk::CHUNK_SIZE * (localY + VoxelChunk::CHUNK_SIZE * localZ)];
}

/**
 * @brief Set voxel state and generate chunk if needed
 * @param x X coordinate in world space
 * @param y Y coordinate in world space
 * @param z Z coordinate in world space
 * @param value New voxel state
 */
void VoxelTerrain::setVoxel(int x, int y, int z, bool value) {
    int chunkX = x / VoxelChunk::CHUNK_SIZE;
    int chunkY = y / VoxelChunk::CHUNK_SIZE;
    int chunkZ = z / VoxelChunk::CHUNK_SIZE;
    
    uint64_t key = getChunkKey(chunkX, chunkY, chunkZ);
    if (m_Chunks.find(key) == m_Chunks.end()) {
        generateChunk(chunkX, chunkY, chunkZ);
    }
}

/**
 * @brief Get chunk at specified coordinates
 * @param chunkX X coordinate in chunk space
 * @param chunkY Y coordinate in chunk space
 * @param chunkZ Z coordinate in chunk space
 * @return VoxelChunk* Pointer to chunk or nullptr if not found
 */
VoxelChunk* VoxelTerrain::getChunk(int chunkX, int chunkY, int chunkZ) const {
    uint64_t key = getChunkKey(chunkX, chunkY, chunkZ);
    auto it = m_Chunks.find(key);
    return it != m_Chunks.end() ? it->second.get() : nullptr;
}

uint64_t VoxelTerrain::getChunkKey(int x, int y, int z) {
    return ((uint64_t)x & 0xFFFFF) | 
        (((uint64_t)y & 0xFFFFF) << 20) |
        (((uint64_t)z & 0xFFFFF) << 40);
}

void VoxelTerrain::SaveHeightmapDebug(int chunkX, int chunkZ) {
    const int mapSize = VoxelChunk::CHUNK_SIZE * 3;
    std::vector<uint8_t> heightData(mapSize * mapSize);
    
    for (int x = 0; x < mapSize; x++) {
        for (int z = 0; z < mapSize; z++) {
            float wx = ((chunkX * VoxelChunk::CHUNK_SIZE) + x) * m_NoiseScale;
            float wz = ((chunkZ * VoxelChunk::CHUNK_SIZE) + z) * m_NoiseScale;
            
            // Generate noise exactly as done in chunk generation
            float continentNoise = m_NoiseGenerator.noise(wx * 0.5f, wz * 0.5f) * 2.0f - 1.0f;
            float terrainNoise = (m_NoiseGenerator.noise(wx * 2.0f, wz * 2.0f) * 2.0f - 1.0f) * 0.5f;
            float detailNoise = (m_NoiseGenerator.noise(wx * 4.0f, wz * 4.0f) * 2.0f - 1.0f) * 0.25f;
            
            // Calculate height as done in chunk generation
            float combinedNoise = continentNoise + terrainNoise + detailNoise;
            float height = ((combinedNoise + 1.0f) * 32.0f) + m_WaterLevel;
            
            // Convert to grayscale (0-255)
            // Map height range 0-128 to 0-255
            uint8_t pixelValue = static_cast<uint8_t>(std::min(255.0f, std::max(0.0f, (height / 128.0f) * 255.0f)));
            heightData[x + z * mapSize] = pixelValue;

            // Debug first few values
            if (x < 2 && z < 2) {
                LOG_TRACE_CONCAT("Heightmap ",x,z,", Height: ",height," Pixel: ",pixelValue);
            }
        }
    }
    
    char filename[100];
    snprintf(filename, sizeof(filename), "heightmap_chunk_%d_%d.bmp", chunkX, chunkZ);
    BMPWriter::SaveGrayscaleBMP(filename, heightData, mapSize, mapSize);
}