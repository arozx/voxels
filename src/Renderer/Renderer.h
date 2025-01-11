#pragma once
#include "Buffer.h"
#include "../Shader/Shader.h"
#include <queue>
#include <glad/glad.h>
#include "../Camera/OrthographicCamera.h"
#include "../Camera/PerspectiveCamera.h"

namespace Engine {
    class Shader;
    class VertexArray;
    class OrthographicCamera;

    struct Transform {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        glm::mat4 GetModelMatrix() const {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, scale);
            return model;
        }
    };

    struct RenderCommand {
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<Shader> shader;
        GLenum primitiveType;
        Transform transform;
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
            const std::shared_ptr<Shader>& shader,
            GLenum primitiveType = GL_TRIANGLES);
        void Submit(const std::shared_ptr<VertexArray>& vertexArray,
            const std::shared_ptr<Shader>& shader,
            const Transform& transform,
            GLenum primitiveType = GL_TRIANGLES);
        void Flush();

        std::shared_ptr<OrthographicCamera>& GetCamera() { return m_Camera; }
        void SetCamera(const std::shared_ptr<OrthographicCamera>& camera) { m_Camera = camera; }

        void SetCameraType(CameraType type);
        CameraType GetCameraType() const { return m_CameraType; }

        std::shared_ptr<PerspectiveCamera>& GetPerspectiveCamera() { return m_PerspectiveCamera; }
        void SetPerspectiveCamera(const std::shared_ptr<PerspectiveCamera>& camera) { m_PerspectiveCamera = camera; }

    private:
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;
        std::queue<RenderCommand> m_CommandQueue;
        std::shared_ptr<Engine::OrthographicCamera> m_Camera;
        CameraType m_CameraType = CameraType::Orthographic;
        std::shared_ptr<PerspectiveCamera> m_PerspectiveCamera;
    };
}