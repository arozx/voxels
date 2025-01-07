#include "World.h"
#include "Voxel.h"
#include <random>
#include <chrono>
#include <iostream>

// Add the destructor implementation
World::~World() {
    chunks.clear(); // This will automatically delete all chunks through unique_ptr
}

unsigned int generateRandomSeed() {
    std::random_device rd;
    auto time = std::chrono::high_resolution_clock::now();
    auto time_seed = time.time_since_epoch().count();
    
    // Mix the seeds using XOR
    return rd() ^ static_cast<unsigned int>(time_seed);
}

World generateWorld() {
    World world(64, 64, 64);
    unsigned int seed = generateRandomSeed();
    std::cout << "Generated terrain with seed: " << seed << std::endl;
    
    // Choose a random terrain type
    World::TerrainType terrainTypes[] = {
        World::FLAT, World::HILLS, 
        World::MOUNTAINS, World::ISLANDS
    };
    int randomType = seed % 4;
    world.generateTerrain(seed, terrainTypes[randomType]);

    return world;
}

World::World(int width, int height, int depth) 
    : width(width), height(height), depth(depth) {
    glm::vec3 origin(width / 2.0f, height / 2.0f, depth / 2.0f);
    glm::vec3 halfDimension(width / 2.0f, height / 2.0f, depth / 2.0f);
    octree = new Octree(origin, halfDimension);
}

void World::setVoxel(int x, int y, int z, const Material& material) {
    Voxel voxel = { glm::vec3(x, y, z), &material, true };
    octree->insert(voxel);
}

void World::removeVoxel(int x, int y, int z) {
    Voxel voxel;
    if (octree->search(glm::vec3(x, y, z), voxel)) {
        voxel.active = false;
        octree->insert(voxel);
    }
}

std::vector<Voxel> World::getActiveVoxels() const {
    return octree->getAllVoxels();
}

float World::getHeightAt(int x, int z, TerrainType type) {
    double scale = 0.1;
    double heightScale = height * 0.5;
    double value = 0.0;
    
    switch(type) {
        case FLAT:
            value = noise.noise(x * scale, 0, z * scale);
            return (value + 1.0) * 2.0;
            
        case HILLS:
            value = noise.octaveNoise(x * scale, 0, z * scale, 4, 0.5);
            return (value + 1.0) * heightScale;
            
        case MOUNTAINS:
            value = std::abs(noise.octaveNoise(x * scale, 0, z * scale, 6, 0.5));
            return value * height * 0.8;
            
        case ISLANDS:
            double distance = glm::length(glm::vec2(x - width/2, z - depth/2));
            double island = 1.0 - (distance / (width * 0.4));
            value = noise.octaveNoise(x * scale, 0, z * scale, 4, 0.5);
            return std::max(0.0, (value + island) * heightScale);
    }
    
    return 0.0;
}

void World::generateTerrain(unsigned int seed, TerrainType type) {
    noise = PerlinNoise(seed);
    
    for (int x = 0; x < width; x++) {
        for (int z = 0; z < depth; z++) {
            int height = static_cast<int>(getHeightAt(x, z, type));
            
            for (int y = 0; y < height; y++) {
                if (y == height - 1) {
                    // Always set top block to grass
                    setVoxel(x, y, z, Materials::GRASS);
                } else if (y > height * 0.7f) {
                    setVoxel(x, y, z, Materials::DIRT);
                } else {
                    setVoxel(x, y, z, Materials::STONE);
                }
            }
        }
    }

    // Add snow caps only for mountains above certain height
    if (type == MOUNTAINS) {
        for (int x = 0; x < width; x++) {
            for (int z = 0; z < depth; z++) {
                int height = static_cast<int>(getHeightAt(x, z, type));
                if (height > this->height * 0.7) {
                    setVoxel(x, height - 1, z, Materials::SNOW);
                }
            }
        }
    }
}