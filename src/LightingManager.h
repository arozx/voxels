#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H

#include <glm/glm.hpp>
#include <vector>
#include <cmath>

class PointLight {
public:
    glm::vec3 position;
    glm::vec3 color;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;

    PointLight(glm::vec3 pos, glm::vec3 col, float ambient, float diffuse, float specular)
        : position(pos), color(col), ambientStrength(ambient), diffuseStrength(diffuse), specularStrength(specular) {}
};

extern std::vector<PointLight> pointLights;

class LightingManager {
public:
    void updateLightPosition(float deltaTime);
    void setupPointLights();

private:
    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    float lightRadius = 10.0f;
    float lightHeight = 10.0f;
    float lightAngle = 0.0f;
};

#endif
