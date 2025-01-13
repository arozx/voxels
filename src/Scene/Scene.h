#pragma once

#include <pch.h>
#include "../Renderer/Renderer.h"
#include "../Camera/OrthographicCamera.h"

namespace Engine {
    class SceneObject : public std::enable_shared_from_this<SceneObject> {
    public:
        std::string name;
        Transform transform;
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<Material> material;
        std::vector<std::shared_ptr<SceneObject>> children;
        std::weak_ptr<SceneObject> parent;

        SceneObject(const std::string& name = "Object") : name(name) {}
        virtual ~SceneObject() = default;

        void AddChild(const std::shared_ptr<SceneObject>& child) {
            child->parent = shared_from_this();
            children.push_back(child);
        }

        glm::mat4 GetWorldTransform() const {
            glm::mat4 worldTransform = transform.GetModelMatrix();
            if (auto parentPtr = parent.lock()) {
                worldTransform = parentPtr->GetWorldTransform() * worldTransform;
            }
            return worldTransform;
        }
    };

    class Scene : public std::enable_shared_from_this<Scene> {
    public:
        Scene(const std::string& name = "Scene") : m_Name(name) {}
        virtual ~Scene() = default;

        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnActivate() {}
        virtual void OnDeactivate() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender(Renderer& renderer) {
            RenderObject(m_RootObject, renderer);
        }

        std::shared_ptr<SceneObject> CreateObject(const std::string& name = "Object") {
            auto object = std::make_shared<SceneObject>(name);
            if (!m_RootObject) {
                m_RootObject = object;
            }
            return object;
        }

        const std::string& GetName() const { return m_Name; }

    protected:
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

        std::string m_Name;
        std::shared_ptr<SceneObject> m_RootObject;
    };
}