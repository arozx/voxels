#pragma once

#include "Scene.h"

namespace Engine {
    class SceneManager {
    public:
        static SceneManager& Get() {
            static SceneManager instance;
            return instance;
        }

        void AddScene(const std::shared_ptr<Scene>& scene) {
            m_Scenes[scene->GetName()] = scene;
        }

        void SetActiveScene(const std::string& name) {
            if (m_ActiveScene) {
                m_ActiveScene->OnDeactivate();
            }
            
            auto it = m_Scenes.find(name);
            if (it != m_Scenes.end()) {
                m_ActiveScene = it->second;
                m_ActiveScene->OnActivate();
            }
        }

        std::shared_ptr<Scene> GetActiveScene() { return m_ActiveScene; }
        
        void Update(float deltaTime) {
            if (m_ActiveScene) {
                m_ActiveScene->OnUpdate(deltaTime);
            }
        }

        void Render(Renderer& renderer) {
            if (m_ActiveScene) {
                m_ActiveScene->OnRender(renderer);
            }
        }

    private:
        SceneManager() = default;
        std::unordered_map<std::string, std::shared_ptr<Scene>> m_Scenes;
        std::shared_ptr<Scene> m_ActiveScene;
    };
}