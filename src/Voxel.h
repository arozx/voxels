#pragma once
#include <glm/glm.hpp>
#include "Material.h"

struct Voxel {
    glm::vec3 position;
    const Material* material;
    bool active;
};
