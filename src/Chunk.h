#pragma once
#include <array>
#include <glm/glm.hpp>
#include "Voxel.h"
#include "Material.h"
#include <vector>

class Chunk {
public:
    static const int CHUNK_SIZE = 32;
    
    Chunk(const glm::ivec3& position);
    
    // Set a voxel in the chunk (local coordinates 0-31)
    void setVoxel(int x, int y, int z, const Material& material);
    
    // Remove a voxel in the chunk (local coordinates 0-31)
    void removeVoxel(int x, int y, int z);
    
    // Get all active voxels in the chunk
    std::vector<Voxel> getActiveVoxels() const;
    
    // Get chunk position in world space
    glm::ivec3 getPosition() const { return position; }
    
    // Convert local coordinates to world coordinates
    glm::vec3 localToWorld(int x, int y, int z) const;
    
    // Convert world coordinates to local coordinates
    glm::ivec3 worldToLocal(const glm::vec3& worldPos) const;

private:
    bool isInBounds(int x, int y, int z) const;
    
    std::array<std::array<std::array<Voxel*, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE> voxels;
    glm::ivec3 position; // Chunk position in world space (in chunk coordinates)
};