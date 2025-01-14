#include "VoxelTerrain.h"

VoxelTerrain::VoxelTerrain(unsigned int seed) : m_NoiseGenerator(seed) {
}

void VoxelTerrain::generateChunk(int chunkX, int chunkY, int chunkZ) {
    uint64_t key = getChunkKey(chunkX, chunkY, chunkZ);
    auto chunk = std::make_unique<VoxelChunk>(chunkX, chunkY, chunkZ);
    chunk->generate(m_NoiseGenerator, m_TerrainScale);
    m_Chunks[key] = std::move(chunk);
}

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

void VoxelTerrain::setVoxel(int x, int y, int z, bool value) {
    int chunkX = x / VoxelChunk::CHUNK_SIZE;
    int chunkY = y / VoxelChunk::CHUNK_SIZE;
    int chunkZ = z / VoxelChunk::CHUNK_SIZE;
    
    uint64_t key = getChunkKey(chunkX, chunkY, chunkZ);
    if (m_Chunks.find(key) == m_Chunks.end()) {
        generateChunk(chunkX, chunkY, chunkZ);
    }
}

VoxelChunk* VoxelTerrain::getChunk(int chunkX, int chunkY, int chunkZ) const {
    uint64_t key = getChunkKey(chunkX, chunkY, chunkZ);
    auto it = m_Chunks.find(key);
    return it != m_Chunks.end() ? it->second.get() : nullptr;
}