#pragma once

#include <pch.h>

#include "../Camera/CameraTypes.h"
#include "../Camera/OrthographicCamera.h"
#include "../Renderer/RenderableObject.h"
#include "../Renderer/Renderer.h"
#include "../Scene/SceneObject.h"
#include "../TerrainSystem/TerrainSystem.h"

namespace Engine {
class Scene {
   public:
    /**
     * @brief Constructs a new Scene with the specified name.
     *
     * Initializes the scene by creating a root SceneObject and logging creation.
     * Terrain is created only if enabled during OnCreate().
     *
     * @param name A string representing the name of the scene.
     */
    explicit Scene(const std::string &name = "Scene");
    virtual ~Scene();

    /** @brief Called when scene is first created */
    virtual void OnCreate();  // Declaration only

    /** @brief Called when scene becomes active */
    virtual void OnActivate();  // Declaration only

    /** @brief Called when scene becomes inactive */
    virtual void OnDeactivate();  // Declaration only

    /** @brief Called every frame to update scene */
    virtual void OnUpdate(float deltaTime);  // Declaration only

    /** @brief Called every frame to render scene */
    virtual void OnRender(Renderer &renderer);  // Declaration only

    /**
     * @brief Creates a new object in the scene
     * @param name Object identifier
     * @return Pointer to created object
     */
    std::shared_ptr<SceneObject> CreateObject(const std::string &name = "Object");

    /** @return Scene name */
    const std::string &GetName() const { return m_Name; }

    /**
     * @brief Get the TerrainSystem instance
     * @return TerrainSystem* Pointer to the TerrainSystem
     */
    TerrainSystem *GetTerrainSystem() const { return m_TerrainSystem.get(); }  // Added getter

    void SetCameraType(CameraType type) {
        m_CameraType = type;
        LOG_TRACE("Camera type set to: ",
                  type == CameraType::Orthographic ? "Orthographic" : "Perspective");
    }

    CameraType GetCameraType() const { return m_CameraType; }

    void SetTerrainEnabled(bool enabled) { m_EnableTerrain = enabled; }
    bool IsTerrainEnabled() const { return m_EnableTerrain; }

    /**
     * @brief Creates and initializes a terrain system for the scene
     * @return True if terrain was created or already exists
     */
    bool CreateTerrain();  // Declaration only

    /**
     * @brief Find a scene object by name
     * @param name Name of object to find
     * @return Shared pointer to found object or nullptr
     */
    std::shared_ptr<SceneObject> GetObject(const std::string &name) const {
        for (const auto &obj : m_Objects) {
            if (obj && obj->name == name) {
                return obj;
            }
        }
        return nullptr;
    }

    void AddObject(const std::shared_ptr<SceneObject> &object) {
        if (object) {
            m_Objects.push_back(object);
        }
    }

   protected:
    /**
     * @brief Renders an object and its children
     * @param object Object to render
     * @param renderer Renderer to use
     */
    void RenderObject(const std::shared_ptr<SceneObject> &object, Renderer &renderer);

   private:
    std::string m_Name;                                   ///< Scene identifier
    std::shared_ptr<SceneObject> m_RootObject;            ///< Root of scene hierarchy
    std::unique_ptr<TerrainSystem> m_TerrainSystem;       ///< Terrain management system
    std::vector<std::shared_ptr<SceneObject>> m_Objects;  // Container for all scene objects
    Renderer *m_Renderer = nullptr;                       // Pointer to renderer instance
    CameraType m_CameraType = CameraType::Orthographic;
    bool m_EnableTerrain = false;  // New flag
};
}