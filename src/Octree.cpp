#include "Octree.h"

Octree::Octree(glm::vec3 origin, glm::vec3 halfDimension)
    : origin(origin), halfDimension(halfDimension), voxel(nullptr) {
    for (int i = 0; i < 8; ++i) {
        children[i] = nullptr;
    }
}

Octree::~Octree() {
    delete voxel;
    for (int i = 0; i < 8; ++i) {
        delete children[i];
    }
}

int Octree::getOctantContainingPoint(const glm::vec3& point) const {
    int oct = 0;
    if (point.x >= origin.x) oct |= 4;
    if (point.y >= origin.y) oct |= 2;
    if (point.z >= origin.z) oct |= 1;
    return oct;
}

bool Octree::isLeafNode() const {
    return children[0] == nullptr;
}

void Octree::insert(const Voxel& voxel) {
    if (isLeafNode()) {
        if (this->voxel == nullptr) {
            this->voxel = new Voxel(voxel);
        } else {
            Voxel* oldVoxel = this->voxel;
            this->voxel = nullptr;

            for (int i = 0; i < 8; ++i) {
                glm::vec3 newOrigin = origin;
                newOrigin.x += halfDimension.x * (i & 4 ? 0.5f : -0.5f);
                newOrigin.y += halfDimension.y * (i & 2 ? 0.5f : -0.5f);
                newOrigin.z += halfDimension.z * (i & 1 ? 0.5f : -0.5f);
                children[i] = new Octree(newOrigin, halfDimension * 0.5f);
            }

            children[getOctantContainingPoint(oldVoxel->position)]->insert(*oldVoxel);
            children[getOctantContainingPoint(voxel.position)]->insert(voxel);
        }
    } else {
        int octant = getOctantContainingPoint(voxel.position);
        children[octant]->insert(voxel);
    }
}

bool Octree::search(const glm::vec3& point, Voxel& result) const {
    if (isLeafNode()) {
        if (voxel != nullptr && voxel->position == point) {
            result = *voxel;
            return true;
        }
        return false;
    }

    int octant = getOctantContainingPoint(point);
    return children[octant]->search(point, result);
}

std::vector<Voxel> Octree::getAllVoxels() const {
    std::vector<Voxel> voxels;
    if (isLeafNode()) {
        if (voxel != nullptr) {
            voxels.push_back(*voxel);
        }
    } else {
        for (int i = 0; i < 8; ++i) {
            std::vector<Voxel> childVoxels = children[i]->getAllVoxels();
            voxels.insert(voxels.end(), childVoxels.begin(), childVoxels.end());
        }
    }
    return voxels;
}
