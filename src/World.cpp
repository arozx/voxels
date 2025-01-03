#include "World.h"

World::World(int width, int height, int depth) 
    : width(width), height(height), depth(depth) {
    voxels.resize(width * height * depth);
    for (auto& voxel : voxels) {
        voxel.active = false;
        voxel.color = glm::vec3(1.0f);
    }
}

void World::setVoxel(int x, int y, int z, const glm::vec3& color) {
    int index = getIndex(x, y, z);
    if (index >= 0) {
        voxels[index].active = true;
        voxels[index].position = glm::vec3(x, y, z);
        voxels[index].color = color;
    }
}

void World::removeVoxel(int x, int y, int z) {
    int index = getIndex(x, y, z);
    if (index >= 0) {
        voxels[index].active = false;
    }
}

std::vector<Voxel> World::getActiveVoxels() const {
    std::vector<Voxel> activeVoxels;
    for (const auto& voxel : voxels) {
        if (voxel.active) {
            activeVoxels.push_back(voxel);
        }
    }
    return activeVoxels;
}

int World::getIndex(int x, int y, int z) const {
    if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
        return -1;
    }
    return x + width * (y + height * z);
}
