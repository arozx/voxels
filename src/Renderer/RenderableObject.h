#pragma once

#include "RenderObject.h"
#include "Renderer.h"

namespace Engine {
    /**
     * @brief Interface for objects that can be rendered
     * 
     * Provides a base class for any object that can be rendered
     * using the rendering system.
     */
    class RenderableObject {
    public:
        virtual ~RenderableObject() = default;
        
        /**
         * @brief Called when object should be rendered
         * @param renderer Reference to the renderer
         */
        virtual void OnRender(Renderer& renderer) {
            if (m_RenderObject) {
                renderer.Submit(
                    m_RenderObject->GetVertexArray(),
                    m_RenderObject->GetMaterial(),
                    m_RenderObject->GetTransform()
                );
            }
        }

        /** @return Reference to the render object */
        RenderObject& GetRenderObject() { return *m_RenderObject; }

        /**
         * @brief Sets the render object for this object
         * @param renderObject New render object
         */
        void SetRenderObject(std::unique_ptr<RenderObject> renderObject) { m_RenderObject = std::move(renderObject); }

    protected:
        std::unique_ptr<RenderObject> m_RenderObject;  ///< Rendering data
    };
}