#include <pch.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "VertexArray.h"
#include "Material.h"
#include "../Shader/Shader.h"
#include "../Camera/OrthographicCamera.h"
#include "../Core/TaskSystem.h"

namespace Engine {
    /**
     * @brief The Renderer class handles all rendering operations in the engine
     */
    Renderer::Renderer() {}

    Renderer::~Renderer() {}

    /**
     * @brief Initialize the renderer and graphics context
     * @details Sets up GLAD and creates camera instances
     */
    void Renderer::Initialize() {
        // Initialize OpenGL
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            ASSERT(false && "Failed to initialize GLAD");
            return;
        }

        // Setup OpenGL state
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Initialize cameras
        m_Camera = std::make_shared<OrthographicCamera>(-1.6f, 1.6f, -0.9f, 0.9f);
        m_PerspectiveCamera = std::make_shared<PerspectiveCamera>(45.0f, 1280.0f/720.0f);
        
        ASSERT(m_Camera != nullptr && "Failed to create orthographic camera");
        ASSERT(m_PerspectiveCamera != nullptr && "Failed to create perspective camera");
    }

    /**
     * @brief Submit a render command to the queue
     * @param vertexArray The vertex array object to render
     * @param material The material to use for rendering
     * @param transform The transform matrix for the object
     * @param primitiveType The OpenGL primitive type to render
     */
    void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray,
        const std::shared_ptr<Material>& material,
        const Transform& transform,
        GLenum primitiveType) 
    {
        // Assert valid parameters
        ASSERT(vertexArray != nullptr && "Null vertex array submitted");
        ASSERT(material != nullptr && "Null material submitted");
        ASSERT(material->GetShader() != nullptr && "Material has null shader");
        
        // Assert valid primitive type
        ASSERT(primitiveType == GL_TRIANGLES || primitiveType == GL_LINES || 
               primitiveType == GL_POINTS && "Invalid primitive type");

        // Don't bind or set uniforms here, just submit the command
        RenderCommand command;
        command.vertexArray = vertexArray;
        command.material = material;
        command.primitiveType = primitiveType;
        command.transform = transform;
        
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        m_CommandQueue.push(command);
    }

    /**
     * @brief Process and execute all queued render commands
     * @details Processes commands in parallel using the TaskSystem and renders on the main thread
     */
    void Renderer::Flush() {
        // Assert we're not recursively processing frames
        if (m_ProcessingFrame.exchange(true)) {
            ASSERT(false && "Already processing a frame");
            return; // Already processing a frame
        }

        std::vector<PreprocessedRenderCommand> commands;
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            while (!m_CommandQueue.empty()) {
                commands.push_back({
                    m_CommandQueue.front().vertexArray,
                    m_CommandQueue.front().material,
                    m_CommandQueue.front().primitiveType,
                    m_CommandQueue.front().transform.GetModelMatrix()
                });
                m_CommandQueue.pop();
            }
        }

        static bool taskSystemInitialized = false;
        if (!taskSystemInitialized) {
            TaskSystem::Get().Initialize();
            taskSystemInitialized = true;
        }

        // Process commands in parallel using TaskSystem
        std::vector<std::future<void>> futures;
        const size_t batchSize = 64; // Number of commands to process per task
        
        for (size_t i = 0; i < commands.size(); i += batchSize) {
            size_t end = std::min(i + batchSize, commands.size());
            futures.push_back(TaskSystem::Get().EnqueueTask([this, &commands, i, end]() {
                for (size_t j = i; j < end; j++) {
                    auto& cmd = commands[j];
                    // Do CPU-intensive work here (transformations, frustum culling, etc.)
                }
            }));
        }

        for (auto& future : futures) {
            future.wait();
        }

        // Execute render commands on main thread
        for (const auto& command : commands) {
            // Assert required components are valid before rendering
            ASSERT(command.vertexArray->GetIndexBuffer() != nullptr && "Null index buffer");
            ASSERT(command.vertexArray->GetIndexBuffer()->GetCount() > 0 && "Empty index buffer");
            
            command.material->Bind();
            auto shader = command.material->GetShader();
            ASSERT(shader != nullptr && "Null shader in material");
            
            if (m_CameraType == CameraType::Orthographic) {
                shader->SetMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
            } else {
                shader->SetMat4("u_ViewProjection", m_PerspectiveCamera->GetViewProjectionMatrix());
            }
            shader->SetMat4("u_Model", command.modelMatrix);
            
            command.vertexArray->Bind();
            glDrawElements(command.primitiveType, 
                command.vertexArray->GetIndexBuffer()->GetCount(), 
                GL_UNSIGNED_INT, 
                nullptr);
                
            command.vertexArray->Unbind();
            command.material->Unbind();
        }

        m_ProcessingFrame = false;
    }

    /**
     * @brief Trigger a draw operation by flushing the command queue
     */
    void Renderer::Draw() {
        Flush();
        if (Profiler::Get().IsProfilingFrames()) {
            Profiler::Get().EndFrame();
        }
    }

    /**
     * @brief Set the active camera type for rendering
     * @param type The camera type to use (Orthographic or Perspective)
     */
    void Renderer::SetCameraType(CameraType type) {
        // Assert valid camera type
        ASSERT(type == CameraType::Orthographic || type == CameraType::Perspective && 
               "Invalid camera type");
        m_CameraType = type;
    }
}