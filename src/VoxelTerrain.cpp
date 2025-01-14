#include "VoxelTerrain.h"

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