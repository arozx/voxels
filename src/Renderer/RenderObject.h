
#pragma once

#include "VertexArray.h"
#include "Material.h"
#include "../Core/Transform.h"

namespace Engine {
    class RenderObject {
    public:
        RenderObject() = default;
        RenderObject(const std::shared_ptr<VertexArray>& vertexArray, 
                    const std::shared_ptr<Material>& material,
                    const Transform& transform = Transform())
            : m_VertexArray(vertexArray), m_Material(material), m_Transform(transform) {}

        const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; }
        const std::shared_ptr<Material>& GetMaterial() const { return m_Material; }
        Transform& GetTransform() { return m_Transform; }

        void SetVertexArray(const std::shared_ptr<VertexArray>& vertexArray) { m_VertexArray = vertexArray; }
        void SetMaterial(const std::shared_ptr<Material>& material) { m_Material = material; }

    private:
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<Material> m_Material;
        Transform m_Transform;
    };
}