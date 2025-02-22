#pragma once
#include <pch.h>

#include "../Core/Transform.h"
#include "../Renderer/Material.h"
#include "../Renderer/RenderableObject.h"
#include "../Renderer/VertexArray.h"

namespace Engine {

class SceneObject : public RenderableObject {
   public:
    SceneObject(const std::string& objectName = "Object") : name(objectName) {}
    virtual ~SceneObject() = default;

    void OnRender(Renderer& renderer) override {
        if (m_Mesh && m_Material) {
            renderer.Submit(m_Mesh, m_Material, m_Transform.GetModelMatrix());
        }
        // Render children
        for (const auto& child : children) {
            if (child) child->OnRender(renderer);
        }
    }

    void AddChild(const std::shared_ptr<SceneObject>& child) { children.push_back(child); }

    // Fix: Return by value instead of reference
    glm::mat4 GetWorldTransform() const { return m_Transform.GetModelMatrix(); }

    void SetMesh(const std::shared_ptr<VertexArray>& mesh) { m_Mesh = mesh; }
    void SetMaterial(const std::shared_ptr<Material>& material) { m_Material = material; }

    const std::shared_ptr<VertexArray>& GetMesh() const { return m_Mesh; }
    const std::shared_ptr<Material>& GetMaterial() const { return m_Material; }

    Transform& GetTransform() { return m_Transform; }
    const Transform& GetTransform() const { return m_Transform; }

    std::string name;
    std::vector<std::shared_ptr<SceneObject>> children;

   protected:
    Transform m_Transform;
    std::shared_ptr<VertexArray> m_Mesh;
    std::shared_ptr<Material> m_Material;
};

}  // namespace Engine