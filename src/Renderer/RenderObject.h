#pragma once

#include "VertexArray.h"
#include "Material.h"
#include "../Core/Transform.h"

namespace Engine {
    /** @brief Class representing a renderable object with geometry, material, and transform
     *
     *  RenderObject combines the necessary components for rendering: vertex data,
     *  material properties, and transformation information.
     */
    class RenderObject {
    public:
        /** @brief Default constructor */
        RenderObject() = default;

        /** @brief Constructs a render object with specified components
         *  @param vertexArray The vertex array containing geometry data
         *  @param material The material to be applied
         *  @param transform The initial transform of the object
         */
        RenderObject(const std::shared_ptr<VertexArray>& vertexArray, 
                    const std::shared_ptr<Material>& material,
                    const Transform& transform = Transform())
            : m_VertexArray(vertexArray), m_Material(material), m_Transform(transform) {}

        /** @brief Gets the vertex array
         *  @return Reference to the vertex array shared pointer
         */
        const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; }

        /** @brief Gets the material
         *  @return Reference to the material shared pointer
         */
        const std::shared_ptr<Material>& GetMaterial() const { return m_Material; }

        /** @brief Gets the transform
         *  @return Reference to the transform object
         */
        Transform& GetTransform() { return m_Transform; }

        /** @brief Sets the vertex array
         *  @param vertexArray New vertex array to use
         */
        void SetVertexArray(const std::shared_ptr<VertexArray>& vertexArray) { m_VertexArray = vertexArray; }

        /** @brief Sets the material
         *  @param material New material to use
         */
        void SetMaterial(const std::shared_ptr<Material>& material) { m_Material = material; }

    private:
        std::shared_ptr<VertexArray> m_VertexArray; ///< The vertex array containing geometry data
        std::shared_ptr<Material> m_Material;       ///< The material applied to this object
        Transform m_Transform;                       ///< The transform of this object
    };
}