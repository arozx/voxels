#pragma once

#include <pch.h>

#include "../Camera/CameraTypes.h"
#include "../Camera/OrthographicCamera.h"
#include "../Renderer/Renderer.h"

namespace Engine {
class SceneObject;
class Renderer;
class TerrainSystem;  // Forward declaration

/**
 * @brief Represents an object in the scene hierarchy
 *
 * Provides transformation, rendering capabilities and parent-child relationships
 * for objects in the scene graph.
 */
class SceneObject : public std::enable_shared_from_this<SceneObject> {
   public:
    std::string name;                                    ///< Object identifier
    Transform transform;                                 ///< Local transformation
    std::shared_ptr<VertexArray> vertexArray;            ///< Mesh data
    std::shared_ptr<Material> material;                  ///< Rendering material
    std::vector<std::shared_ptr<SceneObject>> children;  ///< Child objects
    std::weak_ptr<SceneObject> parent;                   ///< Parent object

    /**
     * @brief Creates a scene object
     * @param name Identifier for the object
     */
    explicit SceneObject(const std::string &name = "Object");
    virtual ~SceneObject() = default;

    /**
     * @brief Adds a child object to this object
     * @param child Object to add as child
     */
    void AddChild(const std::shared_ptr<SceneObject> &child);

    /**
     * @brief Calculates the world transformation matrix
     * @return Combined transformation of object and all parents
     */
    glm::mat4 GetWorldTransform() const;
    };

    /**
     * @brief Base class for game scenes
     * 
     * Manages a hierarchy of objects and provides lifecycle events
     * for scene management.
     */
    class Scene : public std::enable_shared_from_this<Scene> {
    public:
        /**
         * @brief Creates a new scene
         * @param name Scene identifier
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
    };
}