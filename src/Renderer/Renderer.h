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

    /**
     * @brief Structure containing information for a render command
     */
    struct RenderCommand {
        std::shared_ptr<VertexArray> vertexArray;    ///< Vertex array to render
        std::shared_ptr<Material> material;          ///< Material to use
        GLenum primitiveType;                        ///< OpenGL primitive type
        glm::mat4 transformMatrix;                   ///< Transform
    };

    /**
     * @brief Preprocessed version of render command with computed model matrix
     */
    struct PreprocessedRenderCommand {
        std::shared_ptr<VertexArray> vertexArray;    ///< Vertex array to render
        std::shared_ptr<Material> material;          ///< Material to use
        GLenum primitiveType;                        ///< OpenGL primitive type
        glm::mat4 modelMatrix;                       ///< Pre-computed model matrix
    };

    /**
     * @brief Structure representing a low-level GL command
     */
    struct GLCommand {
        /**
         * @brief Types of GL commands that can be executed
         */
        enum class Type {
            BindMaterial,        ///< Bind a material
            SetUniform,         ///< Set a uniform value
            DrawElements,       ///< Draw elements command
            UnbindMaterial,     ///< Unbind a material
            BindVertexArray,    ///< Bind a vertex array
            UnbindVertexArray   ///< Unbind a vertex array
        };

        Type type;                                   ///< Type of GL command
        std::shared_ptr<Material> material;          ///< Material for binding/unbinding
        std::shared_ptr<VertexArray> vertexArray;    ///< Vertex array for binding/unbinding
        glm::mat4 matrix;                           ///< Matrix for uniform setting
        std::string uniformName;                    ///< Name of uniform to set
        GLenum primitiveType;                        ///< Primitive type for drawing
        uint32_t count;                             ///< Element count for drawing
    };

    /**
     * @brief Main renderer class handling all rendering operations
     * @details Manages render queues, cameras, and graphics state
     */
    class Renderer {
    public:
        /**
         * @brief Camera types supported by the renderer
         */
        enum class CameraType {
            Orthographic,    ///< 2D orthographic camera
            Perspective      ///< 3D perspective camera
        };

        Renderer();
        ~Renderer();

        void Initialize();

        /**
         * @brief Trigger a draw operation
         */
        void Draw();

        /**
         * @brief Submit an object for rendering
         * @param vertexArray Vertex array containing geometry
         * @param material Material to use for rendering
         * @param transformMatrix Transform matrix of the object
         * @param primitiveType Type of primitives to render
         */
        void Submit(const std::shared_ptr<VertexArray>& vertexArray,
                    const std::shared_ptr<Material>& material,
                    const glm::mat4& transformMatrix = glm::mat4(1.0f),
                    GLenum primitiveType = GL_TRIANGLES);

        /**
         * @brief Process and execute all queued render commands
         */
        void Flush();

        /**
         * @brief Get the singleton instance of the renderer
         * @return Reference to the renderer instance
         */
        static Renderer& Get() {
            static Renderer instance;
            return instance;
        }

        /**
         * @brief Get the current orthographic camera
         * @return Reference to the orthographic camera
         */
        std::shared_ptr<OrthographicCamera>& GetCamera() { return m_Camera; }

        /**
         * @brief Set the orthographic camera
         * @param camera Shared pointer to the orthographic camera
         */
        void SetCamera(const std::shared_ptr<OrthographicCamera>& camera) { m_Camera = camera; }

        /**
         * @brief Set the camera type
         * @param type Camera type to set
         */
        void SetCameraType(CameraType type);

        /**
         * @brief Get the current camera type
         * @return Current camera type
         */
        CameraType GetCameraType() const { return m_CameraType; }

        /**
         * @brief Get the current perspective camera
         * @return Reference to the perspective camera
         */
        std::shared_ptr<PerspectiveCamera>& GetPerspectiveCamera() { return m_PerspectiveCamera; }

        /**
         * @brief Set the perspective camera
         * @param camera Shared pointer to the perspective camera
         */
        void SetPerspectiveCamera(const std::shared_ptr<PerspectiveCamera>& camera) { m_PerspectiveCamera = camera; }

        /**
         * @brief Set the viewport dimensions
         */
        void SetViewport(int x, int y, int width, int height) const {
            glViewport(x, y, width, height);
        }

        /**
         * @brief Clear the screen with specified color
         */
        void Clear(const glm::vec4& color) const {
            glClearColor(color.r, color.g, color.b, color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void Render();

       private:
        std::mutex m_QueueMutex;                     ///< Mutex for command queue access
        std::mutex m_RenderMutex;                    ///< Mutex for render queue access
        std::shared_ptr<Shader> m_Shader;            ///< Current active shader
        std::shared_ptr<VertexArray> m_VertexArray;  ///< Current vertex array
        std::queue<RenderCommand> m_CommandQueue;     ///< Queue of pending render commands
        std::vector<PreprocessedRenderCommand> m_ProcessingQueue;  ///< Commands being processed
        std::vector<PreprocessedRenderCommand> m_RenderQueue;      ///< Commands ready to render
        std::shared_ptr<Engine::OrthographicCamera> m_Camera;      ///< Orthographic camera
        CameraType m_CameraType = CameraType::Orthographic;        ///< Current camera type
        std::shared_ptr<PerspectiveCamera> m_PerspectiveCamera;    ///< Perspective camera
        std::atomic<bool> m_ProcessingFrame{false};                ///< Frame processing flag
    };
}