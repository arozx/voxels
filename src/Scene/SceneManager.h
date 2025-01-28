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

    void AddScene(const std::shared_ptr<Scene>& scene) {
        std::lock_guard<std::mutex> lock(sceneMutex);
        scenes[scene->GetName()] = scene;
    }

    void SetActiveScene(const std::string& name) {
        std::lock_guard<std::mutex> lock(sceneMutex);
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

    std::shared_ptr<Scene> GetActiveScene() const {
        std::lock_guard<std::mutex> lock(sceneMutex);
        return activeScene;
    }

    std::shared_ptr<Scene> GetScene(const std::string& name) const {
        std::lock_guard<std::mutex> lock(sceneMutex);
        auto it = scenes.find(name);
        return it != scenes.end() ? it->second : nullptr;
    }

    bool RemoveScene(const std::string& name) {
        std::lock_guard<std::mutex> lock(sceneMutex);
        if (scenes.find(name) == scenes.end()) {
            LOG_ERROR_CONCAT("Cannot remove scene: ", name, " (not found)");
            return false;
        }
        if (activeScene && activeScene->GetName() == name) {
            activeScene = nullptr;
        }
        return scenes.erase(name) > 0;
    }

    void Update(float deltaTime) {
        std::lock_guard<std::mutex> lock(sceneMutex);
        if (activeScene) {
            activeScene->OnUpdate(deltaTime);
        }
    }

    void Render(Renderer& renderer) {
        std::lock_guard<std::mutex> lock(sceneMutex);
        if (activeScene) {
            activeScene->OnRender(renderer);
        }
    }

   private:
    SceneManager() = default;
    mutable std::mutex sceneMutex;
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> activeScene;
};
}  // namespace Engine