#pragma once
#include "../pch.h"
#include <glad/glad.h>
#include <atomic>
#include "Buffer.h"
#include "Material.h"
#include "../Shader/Shader.h"
#include "../Camera/OrthographicCamera.h"
#include "../Camera/PerspectiveCamera.h"
#include "../Core/Transform.h"

namespace Engine {
    class Shader;
    class VertexArray;
    class OrthographicCamera;
    class Material;

    struct RenderCommand {
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<Material> material;
        GLenum primitiveType;
        Transform transform;
    };

    struct PreprocessedRenderCommand {
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<Material> material;
        GLenum primitiveType;
        glm::mat4 modelMatrix;
    };

    struct GLCommand {
        enum class Type {
            BindMaterial,
            SetUniform,
            DrawElements,
            UnbindMaterial,
            BindVertexArray,
            UnbindVertexArray
        };

        Type type;
        std::shared_ptr<Material> material;
        std::shared_ptr<VertexArray> vertexArray;
        glm::mat4 matrix;
        std::string uniformName;
        GLenum primitiveType;
        uint32_t count;
    };

    class Renderer {
    public:
        enum class CameraType {
            Orthographic,
            Perspective
        };

        Renderer();
        ~Renderer();

        void Init();
        void Draw();
        void Submit(const std::shared_ptr<VertexArray>& vertexArray,
            const std::shared_ptr<Material>& material,
            const Transform& transform = Transform(),
            GLenum primitiveType = GL_TRIANGLES);
        void Flush();

        static Renderer& Get() {
            static Renderer instance;
            return instance;
        }

        std::shared_ptr<OrthographicCamera>& GetCamera() { return m_Camera; }
        void SetCamera(const std::shared_ptr<OrthographicCamera>& camera) { m_Camera = camera; }

        void SetCameraType(CameraType type);
        CameraType GetCameraType() const { return m_CameraType; }

        std::shared_ptr<PerspectiveCamera>& GetPerspectiveCamera() { return m_PerspectiveCamera; }
        void SetPerspectiveCamera(const std::shared_ptr<PerspectiveCamera>& camera) { m_PerspectiveCamera = camera; }

    private:
        std::mutex m_QueueMutex;
        std::mutex m_RenderMutex;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;
        std::queue<RenderCommand> m_CommandQueue;
        std::vector<PreprocessedRenderCommand> m_ProcessingQueue;
        std::vector<PreprocessedRenderCommand> m_RenderQueue;
        std::shared_ptr<Engine::OrthographicCamera> m_Camera;
        CameraType m_CameraType = CameraType::Orthographic;
        std::shared_ptr<PerspectiveCamera> m_PerspectiveCamera;
        std::atomic<bool> m_ProcessingFrame{false};
    };
}