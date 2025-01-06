#include "LightingManager.h"

std::vector<PointLight> pointLights;

void LightingManager::updateLightPosition(float deltaTime) {
    // Make light orbit around the center
    lightAngle += deltaTime * 0.5f;
    lightPos.x = sin(lightAngle) * lightRadius + 16.0f;
    lightPos.z = cos(lightAngle) * lightRadius + 16.0f;
    lightPos.y = lightHeight;

    if (!pointLights.empty()) {
        pointLights[0].position = lightPos;
    }
}

void LightingManager::setupPointLights() {
    pointLights.push_back({ glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.1f, 1.0f, 0.5f });
    pointLights.push_back({ glm::vec3(-1.2f, 1.0f, -2.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.1f, 1.0f, 0.5f });
    pointLights.push_back({ glm::vec3(1.2f, -1.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 1.0f, 0.5f });
    pointLights.push_back({ glm::vec3(-1.2f, -1.0f, -2.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.1f, 1.0f, 0.5f });
}
