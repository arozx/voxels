#pragma once

#include "Scene.h"

namespace Engine {
    /**
     * @brief Manages scene lifecycle and transitions
     * 
     * Singleton class that handles scene switching, updates,
     * and rendering of the active scene.
     */
    class SceneManager {
    public:
        /** @return Reference to singleton instance */
        static SceneManager& Get() {
            static SceneManager instance;
            return instance;
        }

        /**
         * @brief Registers a scene with the manager
         * @param scene Scene to register
         */
        void AddScene(const std::shared_ptr<Scene>& scene) {
            m_Scenes[scene->GetName()] = scene;
        }

        /**
         * @brief Changes the active scene
         * @param name Name of scene to activate
         */
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

        /** @return Currently active scene */
        std::shared_ptr<Scene> GetActiveScene() { return m_ActiveScene; }
        
        /**
         * @brief Updates the active scene
         * @param deltaTime Time since last update
         */
        void Update(float deltaTime) {
            if (m_ActiveScene) {
                m_ActiveScene->OnUpdate(deltaTime);
            }
        }

        /**
         * @brief Renders the active scene
         * @param renderer Renderer to use
         */
        void Render(Renderer& renderer) {
            if (m_ActiveScene) {
                m_ActiveScene->OnRender(renderer);
            }
        }

    private:
        SceneManager() = default;
        std::unordered_map<std::string, std::shared_ptr<Scene>> m_Scenes;  ///< Available scenes
        std::shared_ptr<Scene> m_ActiveScene;                              ///< Currently active scene
    };
}