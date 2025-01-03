#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Voxel {
    glm::vec3 position;
    glm::vec3 color;
    bool active;
};

class World {
public:
    World(int width, int height, int depth);
    void setVoxel(int x, int y, int z, const glm::vec3& color);
    void removeVoxel(int x, int y, int z);
    std::vector<Voxel> getActiveVoxels() const;

private:
    std::vector<Voxel> voxels;
    int width, height, depth;
    int getIndex(int x, int y, int z) const;
};
