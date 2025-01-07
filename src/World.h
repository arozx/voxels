#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "PerlinNoise.h"
#include "Material.h"
#include "Octree.h"
#include "Voxel.h"
#include "Chunk.h"
#include <unordered_map>
#include <vector>

namespace std {
    template<>
    struct hash<glm::ivec3> {
        size_t operator()(const glm::ivec3& v) const {
            // Combine the hash of the components
            size_t h1 = hash<int>()(v.x);
            size_t h2 = hash<int>()(v.y);
            size_t h3 = hash<int>()(v.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

class World {
public:
    World(int width, int height, int depth);
    ~World();

    void setVoxel(int x, int y, int z, const Material& material);
    void removeVoxel(int x, int y, int z);
    std::vector<Voxel> getActiveVoxels() const;
    
    enum TerrainType {
        FLAT,
        HILLS,
        MOUNTAINS,
        ISLANDS
    };
    
    void generateTerrain(unsigned int seed = 123, TerrainType type = MOUNTAINS);

private:
    std::unordered_map<glm::ivec3, Chunk*> chunks;
    
    Chunk* getOrCreateChunk(const glm::ivec3& chunkPos);
    glm::ivec3 worldToChunkPosition(int x, int y, int z) const;

    Octree* octree;
    int width, height, depth;
    PerlinNoise noise;
    
    float getHeightAt(int x, int z, TerrainType type);
};

// Utility function declarations
unsigned int generateRandomSeed();
World generateWorld();
