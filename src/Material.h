#pragma once
#include <glm/glm.hpp>
#include <string>

struct Material {
    std::string name;
    glm::vec3 color;
    float ambient;
    float diffuse;
    float specular;
    int shininess;

    Material(const std::string& name, const glm::vec3& color, 
            float ambient, float diffuse, float specular, int shininess)
        : name(name), color(color), ambient(ambient), 
          diffuse(diffuse), specular(specular), shininess(shininess) {}
};

namespace Materials {
    const Material GRASS("Grass", glm::vec3(0.2f, 0.8f, 0.2f), 0.1f, 1.0f, 0.2f, 4);
    const Material DIRT("Dirt", glm::vec3(0.5f, 0.35f, 0.05f), 0.1f, 0.9f, 0.0f, 1);
    const Material STONE("Stone", glm::vec3(0.5f, 0.5f, 0.5f), 0.1f, 0.7f, 0.8f, 32);
    const Material SNOW("Snow", glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, 1.0f, 0.9f, 64);
    const Material METAL("Metal", glm::vec3(0.7f, 0.7f, 0.7f), 0.1f, 0.7f, 1.0f, 128);
    const Material GOLD("Gold", glm::vec3(1.0f, 0.84f, 0.0f), 0.2f, 0.8f, 1.0f, 64);
    const Material WATER("Water", glm::vec3(0.0f, 0.4f, 0.9f), 0.1f, 0.9f, 1.0f, 256);
}
