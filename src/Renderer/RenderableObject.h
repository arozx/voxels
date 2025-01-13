
#pragma once

#include "RenderObject.h"
#include "Renderer.h"

namespace Engine {
    class RenderableObject {
    public:
        virtual ~RenderableObject() = default;
        
        virtual void OnRender(Renderer& renderer) {
            if (m_RenderObject) {
                renderer.Submit(
                    m_RenderObject->GetVertexArray(),
                    m_RenderObject->GetMaterial(),
                    m_RenderObject->GetTransform()
                );
            }
        }

        RenderObject& GetRenderObject() { return *m_RenderObject; }
        void SetRenderObject(std::unique_ptr<RenderObject> renderObject) { m_RenderObject = std::move(renderObject); }

    protected:
        std::unique_ptr<RenderObject> m_RenderObject;
    };
}