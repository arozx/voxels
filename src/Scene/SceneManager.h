#pragma once

#include <pch.h>

#include "Scene.h"

namespace Engine {
class SceneManager {
   public:
    static SceneManager& Get() {
        static SceneManager instance;
        return instance;
    }

    void AddScene(const std::shared_ptr<Scene>& scene) { scenes[scene->GetName()] = scene; }

    void SetActiveScene(const std::string& name) {
        LOG_TRACE("SetActiveScene called with name = ", name);
        if (scenes.find(name) != scenes.end()) {
            if (activeScene) {
                activeScene->OnDeactivate();
            }
            activeScene = scenes[name];
            activeScene->OnActivate();
            LOG_INFO_CONCAT("Activated scene: ", name);
        } else {
            LOG_ERROR_CONCAT("Failed to set active scene: ", name, " (not found)");
        }
    }

    std::shared_ptr<Scene> GetActiveScene() const { return activeScene; }

    std::shared_ptr<Scene> GetScene(const std::string& name) const {
        auto it = scenes.find(name);
        return it != scenes.end() ? it->second : nullptr;
    }

    bool RemoveScene(const std::string& name) {
        if (activeScene && activeScene->GetName() == name) {
            activeScene = nullptr;
        }
        return scenes.erase(name) > 0;
    }

    void Update(float deltaTime) {
        if (activeScene) {
            activeScene->OnUpdate(deltaTime);
        }
    }

    void Render(Renderer& renderer) {
        if (activeScene) {
            activeScene->OnRender(renderer);
        }
    }

   private:
    SceneManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> activeScene;
};
}  // namespace Engine