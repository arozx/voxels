#pragma once
#include <glm/glm.hpp>
#include "Voxel.h"
#include <vector>

class Octree {
public:
    Octree(glm::vec3 origin, glm::vec3 halfDimension);
    ~Octree();

    void insert(const Voxel& voxel);
    bool search(const glm::vec3& point, Voxel& result) const;
    std::vector<Voxel> getAllVoxels() const;

private:
    glm::vec3 origin;
    glm::vec3 halfDimension;
    Voxel* voxel;
    Octree* children[8];

    int getOctantContainingPoint(const glm::vec3& point) const;
    bool isLeafNode() const;
};
