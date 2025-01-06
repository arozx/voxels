#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "PerlinNoise.h"
#include "Material.h"
#include "Octree.h"
#include "Voxel.h"

class World {
public:
    World(int width, int height, int depth);
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
    Octree* octree;
    int width, height, depth;
    PerlinNoise noise;
    
    float getHeightAt(int x, int z, TerrainType type);
};

// Utility function declarations
unsigned int generateRandomSeed();
World generateWorld();
