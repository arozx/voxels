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

    bool AddScene(const std::shared_ptr<Scene>& scene) {
        if (!scene) {
            LOG_ERROR("Failed to add scene: nullptr provided");
            return false;
        }

        std::lock_guard<std::mutex> lock(sceneMutex);

        const std::string& name = scene->GetName();
        if (name.empty()) {
            LOG_ERROR("Failed to add scene: scene has empty name");
            return false;
        }

        if (scenes.find(name) != scenes.end()) {
            LOG_ERROR("Failed to add scene: ", name, " (already exists)");
            return false;
        }

        scenes[name] = scene;
        LOG_INFO("Added scene: ", name);
        return true;
    }

    bool SetActiveScene(const std::string& name) {
        if (name.empty()) {
            LOG_ERROR("Failed to set active scene: empty name provided");
            return false;
        }

        std::lock_guard<std::mutex> lock(sceneMutex);
        auto it = scenes.find(name);
        if (it != scenes.end()) {
            try {
                if (activeScene) {
                    LOG_TRACE("Deactivating current scene: ", activeScene->GetName());
                    activeScene->OnDeactivate();
                }
                activeScene = it->second;
                LOG_TRACE("Activating scene: ", name);
                activeScene->OnActivate();
                LOG_INFO("Activated scene: ", name);
                return true;
            } catch (const std::exception& e) {
                LOG_ERROR("Exception during scene activation: ", e.what());
                return false;
            }
        } else {
            LOG_ERROR("Failed to set active scene: ", name, " (not found)");
            return false;
        }
    }

    std::shared_ptr<Scene> GetActiveScene() const {
        std::lock_guard<std::mutex> lock(sceneMutex);
        if (!activeScene) {
            LOG_WARN("GetActiveScene: No active scene currently set");
        }
        return activeScene;
    }

    std::shared_ptr<Scene> GetScene(const std::string& name) const {
        if (name.empty()) {
            LOG_ERROR("GetScene: Empty name provided");
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(sceneMutex);
        auto it = scenes.find(name);
        if (it == scenes.end()) {
            LOG_ERROR("GetScene: Scene not found: ", name);
            return nullptr;
        }
        return it->second;
    }

    bool RemoveScene(const std::string& name) {
        if (name.empty()) {
            LOG_ERROR("Failed to remove scene: empty name provided");
            return false;
        }

        std::lock_guard<std::mutex> lock(sceneMutex);
        auto it = scenes.find(name);
        if (it == scenes.end()) {
            LOG_ERROR("Cannot remove scene: ", name, " (not found)");
            return false;
        }

        if (activeScene && activeScene->GetName() == name) {
            LOG_WARN("Removing currently active scene: ", name);
            activeScene = nullptr;
        }

        scenes.erase(it);
        LOG_INFO("Removed scene: ", name);
        return true;
    }

    void Update(float deltaTime) {
        std::lock_guard<std::mutex> lock(sceneMutex);
        if (activeScene) {
            try {
                activeScene->OnUpdate(deltaTime);
            } catch (const std::exception& e) {
                LOG_ERROR("Exception during scene update: ", e.what());
            }
        } else {
            LOG_TRACE("Update: No active scene to update");
        }
    }

    void Render(Renderer& renderer) {
        std::lock_guard<std::mutex> lock(sceneMutex);
        if (activeScene) {
            try {
                activeScene->OnRender(renderer);
            } catch (const std::exception& e) {
                LOG_ERROR("Exception during scene rendering: ", e.what());
            }
        } else {
            LOG_TRACE("Render: No active scene to render");
        }
    }

    size_t GetSceneCount() const {
        std::lock_guard<std::mutex> lock(sceneMutex);
        return scenes.size();
    }

    std::vector<std::string> GetSceneNames() const {
        std::lock_guard<std::mutex> lock(sceneMutex);
        std::vector<std::string> names;
        names.reserve(scenes.size());

        for (const auto& [name, _] : scenes) {
            names.push_back(name);
        }

        return names;
    }

   private:
    SceneManager() = default;
    ~SceneManager() {
        LOG_TRACE("SceneManager shutting down, clearing scenes");
        std::lock_guard<std::mutex> lock(sceneMutex);
        activeScene = nullptr;
        scenes.clear();
    }

    // Prevent copying and assignment
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    mutable std::mutex sceneMutex;
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> activeScene;
};
}  // namespace Engine
