#include "Chunk.h"
#include <vector>

Chunk::Chunk(const glm::ivec3& position) : position(position) {
    // Initialize all voxels to nullptr
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                voxels[x][y][z] = nullptr;
            }
        }
    }
}

void Chunk::setVoxel(int x, int y, int z, const Material& material) {
    if (!isInBounds(x, y, z)) return;
    
    if (voxels[x][y][z] == nullptr) {
        voxels[x][y][z] = new Voxel{
            localToWorld(x, y, z),
            &material,
            true
        };
    } else {
        voxels[x][y][z]->material = &material;
        voxels[x][y][z]->active = true;
    }
}

void Chunk::removeVoxel(int x, int y, int z) {
    if (!isInBounds(x, y, z)) return;
    
    if (voxels[x][y][z] != nullptr) {
        voxels[x][y][z]->active = false;
    }
}

std::vector<Voxel> Chunk::getActiveVoxels() const {
    std::vector<Voxel> activeVoxels;
    
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (voxels[x][y][z] != nullptr && voxels[x][y][z]->active) {
                    activeVoxels.push_back(*voxels[x][y][z]);
                }
            }
        }
    }
    
    return activeVoxels;
}

glm::vec3 Chunk::localToWorld(int x, int y, int z) const {
    return glm::vec3(
        x + position.x * CHUNK_SIZE,
        y + position.y * CHUNK_SIZE,
        z + position.z * CHUNK_SIZE
    );
}

glm::ivec3 Chunk::worldToLocal(const glm::vec3& worldPos) const {
    return glm::ivec3(
        static_cast<int>(worldPos.x) % CHUNK_SIZE,
        static_cast<int>(worldPos.y) % CHUNK_SIZE,
        static_cast<int>(worldPos.z) % CHUNK_SIZE
    );
}

bool Chunk::isInBounds(int x, int y, int z) const {
    return x >= 0 && x < CHUNK_SIZE &&
           y >= 0 && y < CHUNK_SIZE &&
           z >= 0 && z < CHUNK_SIZE;
}