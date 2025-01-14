#pragma once

#include <pch.h>
#include "../Renderer/Renderer.h"
#include "../Camera/OrthographicCamera.h"

namespace Engine {
    /**
     * @brief Represents an object in the scene hierarchy
     * 
     * Provides transformation, rendering capabilities and parent-child relationships
     * for objects in the scene graph.
     */
    class SceneObject : public std::enable_shared_from_this<SceneObject> {
    public:
        std::string name;              ///< Object identifier
        Transform transform;           ///< Local transformation
        std::shared_ptr<VertexArray> vertexArray;  ///< Mesh data
        std::shared_ptr<Material> material;        ///< Rendering material
        std::vector<std::shared_ptr<SceneObject>> children;  ///< Child objects
        std::weak_ptr<SceneObject> parent;         ///< Parent object

        /**
         * @brief Creates a scene object
         * @param name Identifier for the object
         */
        SceneObject(const std::string& name = "Object") : name(name) {}
        virtual ~SceneObject() = default;

        /**
         * @brief Adds a child object to this object
         * @param child Object to add as child
         */
        void AddChild(const std::shared_ptr<SceneObject>& child) {
            child->parent = shared_from_this();
            children.push_back(child);
        }

        /**
         * @brief Calculates the world transformation matrix
         * @return Combined transformation of object and all parents
         */
        glm::mat4 GetWorldTransform() const {
            glm::mat4 worldTransform = transform.GetModelMatrix();
            if (auto parentPtr = parent.lock()) {
                worldTransform = parentPtr->GetWorldTransform() * worldTransform;
            }
            return worldTransform;
        }
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
        Scene(const std::string& name = "Scene") : m_Name(name) {}
        virtual ~Scene() = default;

        /** @brief Called when scene is first created */
        virtual void OnCreate() {}
        /** @brief Called when scene is being destroyed */
        virtual void OnDestroy() {}
        /** @brief Called when scene becomes active */
        virtual void OnActivate() {}
        /** @brief Called when scene becomes inactive */
        virtual void OnDeactivate() {}
        /** @brief Called every frame to update scene */
        virtual void OnUpdate(float deltaTime) {}
        /** @brief Called every frame to render scene */
        virtual void OnRender(Renderer& renderer) {
            RenderObject(m_RootObject, renderer);
        }

        /**
         * @brief Creates a new object in the scene
         * @param name Object identifier
         * @return Pointer to created object
         */
        std::shared_ptr<SceneObject> CreateObject(const std::string& name = "Object") {
            auto object = std::make_shared<SceneObject>(name);
            if (!m_RootObject) {
                m_RootObject = object;
            }
            return object;
        }

        /** @return Scene name */
        const std::string& GetName() const { return m_Name; }

    protected:
        /**
         * @brief Renders an object and its children
         * @param object Object to render
         * @param renderer Renderer to use
         */
        void RenderObject(const std::shared_ptr<SceneObject>& object, Renderer& renderer) {
            if (!object) return;
            
            if (object->vertexArray && object->material) {
                PreprocessedRenderCommand cmd;
                cmd.vertexArray = object->vertexArray;
                cmd.material = object->material;
                cmd.modelMatrix = object->GetWorldTransform();
                cmd.primitiveType = GL_TRIANGLES;
                
                renderer.Submit(cmd.vertexArray, cmd.material, object->transform);
            }

            for (const auto& child : object->children) {
                RenderObject(child, renderer);
            }
        }

        std::string m_Name;                         ///< Scene identifier
        std::shared_ptr<SceneObject> m_RootObject;  ///< Root of scene hierarchy
    };
}