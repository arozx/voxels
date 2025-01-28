#include "Scene.h"

#include <pch.h>

#include "TerrainSystem/TerrainSystem.h"

namespace Engine {
// SceneObject Definitions

SceneObject::SceneObject(const std::string &name) : name(name) {}

void SceneObject::AddChild(const std::shared_ptr<SceneObject> &child) {
    child->parent = shared_from_this();
    children.push_back(child);
}

glm::mat4 SceneObject::GetWorldTransform() const {
    glm::mat4 worldTransform = transform.GetModelMatrix();
    if (auto parentPtr = parent.lock()) {
        worldTransform = parentPtr->GetWorldTransform() * worldTransform;
    }
    return worldTransform;
}

// Scene Definitions

Scene::Scene(const std::string &name) : m_Name(name) {
    m_RootObject = std::make_shared<SceneObject>("Root");
    m_TerrainSystem = std::make_unique<TerrainSystem>();
    LOG_TRACE("Created scene: ", name);
}

std::shared_ptr<SceneObject> Scene::CreateObject(const std::string &name) {
    // Create a new scene object with the given name
    auto object = std::make_shared<SceneObject>(name);

    // Add to managed objects container
    m_Objects.push_back(object);

    // If there's no root object, set this as the root
    if (!m_RootObject) {
        m_RootObject = object;
    }

    LOG_TRACE("Created scene object: ", name);
    return object;
}

Scene::~Scene() {
    // Ensure that all resources are cleaned up
    m_TerrainSystem.reset();
}

void Scene::OnCreate() {
    LOG_TRACE("Initializing scene: ", m_Name);

    if (!m_TerrainSystem) {
        m_TerrainSystem = std::make_unique<TerrainSystem>();
    }

    if (m_TerrainSystem && m_Renderer) {
        m_TerrainSystem->Initialize(*m_Renderer);
    }
}

void Scene::OnActivate() {
    // Empty implementation - can be overridden by derived classes
}

void Scene::OnDeactivate() {
    // Empty implementation - can be overridden by derived classes
}

void Scene::OnUpdate(float deltaTime) {
    if (m_TerrainSystem) {
        m_TerrainSystem->Update(deltaTime);
    }
}

void Scene::OnRender(Renderer &renderer) {
    // Store renderer reference and initialize if needed
    if (!m_Renderer) {
        m_Renderer = &renderer;
        OnCreate();
    }

    // Render terrain first
    if (m_TerrainSystem) {
        m_TerrainSystem->Render(renderer);
    }

    // Then render scene objects
    if (m_RootObject) {
        RenderObject(m_RootObject, renderer);
    }
}

void Scene::RenderObject(const std::shared_ptr<SceneObject> &object, Renderer &renderer) {
    if (!object) return;

    if (object->vertexArray && object->material) {
        PreprocessedRenderCommand cmd;
        cmd.vertexArray = object->vertexArray;
        cmd.material = object->material;
        cmd.modelMatrix = object->GetWorldTransform();
        cmd.primitiveType = GL_TRIANGLES;

        renderer.Submit(cmd.vertexArray, cmd.material, object->transform);
    }

    for (const auto &child : object->children) {
        RenderObject(child, renderer);
    }
}
}  // namespace Engine