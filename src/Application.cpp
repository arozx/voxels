#include <pch.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Application.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"
#include "Events/KeyCodes.h"
#include "Camera/OrthographicCamera.h"
#include "Renderer/VertexArray.h"
#include <imgui.h>
#include "Shader/DefaultShaders.h"
#include "Renderer/MeshTemplates.h"
#include "Debug/Profiler.h"
#include "Scene/SceneManager.h"
#include "UI/ImGuiOverlay.h"

namespace Engine {
    Application::Application() {
        LOG_INFO("Creating Application");
        InitWindow("Voxel Engine", 1280, 720);
        if (!m_Window) {
            LOG_ERROR("Failed to create window!");
            m_Running = false;
            return;
        }
        m_Window->SetContext();

        m_ImGuiLayer = std::make_unique<ImGuiLayer>();
        m_ImGuiLayer->Init(m_Window.get());

        m_Renderer.Init();

        // Initialize terrain system before other rendering objects
        m_TerrainSystem = std::make_unique<TerrainSystem>();
        m_TerrainSystem->Initialize(m_Renderer);  // Make sure TerrainSystem has this method

        // Create triangle
        m_Triangle = std::make_unique<RenderableObject>();
        auto triangleShader = DefaultShaders::LoadSimpleColorShader();
        auto triangleMaterial = std::make_shared<Material>(triangleShader);
        triangleMaterial->SetVector4("u_Color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));

        auto triangleVA = std::shared_ptr<VertexArray>(VertexArray::Create());
        auto triangleVertices = MeshTemplates::GetVertexData(MeshTemplates::Triangle);
        
        std::shared_ptr<VertexBuffer> triangleVB(
            VertexBuffer::Create(triangleVertices.data(), 
            triangleVertices.size() * sizeof(float)));
        
        BufferLayout triangleLayout = {
            { ShaderDataType::Float3, "aPosition" }
        };
        
        triangleVB->SetLayout(triangleLayout);
        triangleVA->AddVertexBuffer(triangleVB);

        std::shared_ptr<IndexBuffer> triangleIB(
            IndexBuffer::Create(MeshTemplates::TriangleIndices.data(), 
            MeshTemplates::TriangleIndices.size()));
        triangleVA->SetIndexBuffer(triangleIB);

        m_Triangle->SetRenderObject(std::make_unique<RenderObject>(
            triangleVA, triangleMaterial, Transform()));

        // Load shared texture
        m_TestTexture = Texture::Create("assets/textures/test.png");

        // Create textured square
        auto createSquare = [this](const glm::vec3& position, 
            const std::shared_ptr<Shader>& shader,
            const glm::vec4& color = glm::vec4(1.0f)) -> std::unique_ptr<RenderableObject> {
            auto object = std::make_unique<RenderableObject>();
            
            auto va = std::shared_ptr<VertexArray>(VertexArray::Create());
            auto vertices = MeshTemplates::TexturedSquare;
            
            std::shared_ptr<VertexBuffer> vb(
                VertexBuffer::Create(vertices.data(), 
                vertices.size() * sizeof(float)));
                
            BufferLayout layout = {
                { ShaderDataType::Float3, "aPosition" },
                { ShaderDataType::Float2, "aTexCoord" }
            };
            
            vb->SetLayout(layout);
            va->AddVertexBuffer(vb);
            
            std::shared_ptr<IndexBuffer> ib(
                IndexBuffer::Create(MeshTemplates::SquareIndices.data(), 
                MeshTemplates::SquareIndices.size()));
            va->SetIndexBuffer(ib);
            
            auto material = std::make_shared<Material>(shader);
            material->SetTexture("u_Texture", m_TestTexture);
            material->SetVector4("u_Color", color);
            
            Transform transform;
            transform.position = position;
            transform.scale = glm::vec3(0.5f);
            
            object->SetRenderObject(std::make_unique<RenderObject>(va, material, transform));
            return object;
        };

        // Create all squares using the helper function
        m_TexturedSquare = createSquare(
            glm::vec3(0.5f, 0.5f, 0.5f),
            DefaultShaders::LoadTexturedShader()
        );

        m_TransparentSquare = createSquare(
            glm::vec3(0.5f, 0.5f, 0.4f),
            DefaultShaders::LoadTexturedShader(),
            glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)
        );

        CreateFileShaderSquare();
        CreatePixelatedSquare();
        CreateWaveDissolveSquare();
        CreateBlurSquare();
        
        m_FPSSamples.resize(FPS_SAMPLE_COUNT, 0.0f);

        m_InputSystem = std::make_unique<InputSystem>(m_Window.get(), m_Renderer);

        m_ImGuiOverlay = std::make_unique<ImGuiOverlay>(m_Window.get());

        // Instead of creating raw rendering resources, create RenderableObjects
        auto createTexturedSquare = [](const glm::vec3& position, const glm::vec4& color) {
            auto object = std::make_unique<RenderableObject>();
            
            auto vertexArray = std::shared_ptr<VertexArray>(VertexArray::Create());
            auto vertices = MeshTemplates::TexturedSquare;
            
            std::shared_ptr<VertexBuffer> vertexBuffer(
                VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float)));
                
            BufferLayout layout = {
                { ShaderDataType::Float3, "aPosition" },
                { ShaderDataType::Float2, "aTexCoord" }
            };
            
            vertexBuffer->SetLayout(layout);
            vertexArray->AddVertexBuffer(vertexBuffer);
            
            std::shared_ptr<IndexBuffer> indexBuffer(
                IndexBuffer::Create(MeshTemplates::SquareIndices.data(), 
                MeshTemplates::SquareIndices.size()));
            vertexArray->SetIndexBuffer(indexBuffer);
            
            auto shader = DefaultShaders::LoadTexturedShader();
            auto material = std::make_shared<Material>(shader);
            material->SetVector4("u_Color", color);
            
            Transform transform;
            transform.position = position;
            transform.scale = glm::vec3(0.5f);
            
            object->SetRenderObject(std::make_unique<RenderObject>(vertexArray, material, transform));
            return object;
        };

        // Create renderable objects
        m_RenderableObjects.push_back(createTexturedSquare(glm::vec3(0.5f), glm::vec4(1.0f)));
        m_RenderableObjects.push_back(createTexturedSquare(glm::vec3(-0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)));
        // Add more objects as needed...
    }

    Application::~Application() {
        if (m_ImGuiLayer) {
            m_ImGuiLayer->Shutdown();
        }
        LOG_INFO("Application Destroyed");
    }

    void Application::Run() {
        PROFILE_FUNCTION();
        LOG_INFO("Application Starting...");
        
        Profiler::Get().BeginSession("Runtime");
        
        bool ImGuiEnabled = true;
        float lastFrameTime = 0.0f;

        while (m_Running && m_Window) {
            float time = (float)glfwGetTime();
            float deltaTime = time - lastFrameTime;
            lastFrameTime = time;

            m_TerrainSystem->Update(deltaTime);
            m_InputSystem->Update(deltaTime);
            SceneManager::Get().Update(deltaTime);

            // Toggle FPS counter with F3
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_F3) == GLFW_PRESS) {
                static float lastPress = 0.0f;
                if (time - lastPress > 0.2f) { // Debounce
                    m_ShowFPSCounter = !m_ShowFPSCounter;
                    lastPress = time;
                }
            }

            UpdateFPSCounter(deltaTime, time);

            // Update active square transform
            if (m_TexturedSquare) {
                auto& transform = m_TexturedSquare->GetRenderObject().GetTransform();
                
                // Update scale
                transform.scale = glm::vec3(1.0f);

                // WASD movement
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_A) == GLFW_PRESS)
                    transform.position.x -= 2.0f * deltaTime;
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_D) == GLFW_PRESS)
                    transform.position.x += 2.0f * deltaTime;
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_W) == GLFW_PRESS)
                    transform.position.y += 2.0f * deltaTime;
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_S) == GLFW_PRESS)
                    transform.position.y -= 2.0f * deltaTime;

                // QE rotation
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_Q) == GLFW_PRESS)
                    transform.rotation.z += 2.0f * deltaTime;
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_E) == GLFW_PRESS)
                    transform.rotation.z -= 2.0f * deltaTime;
            }

            // Update wave dissolve effect
            if (m_WaveDissolveSquare) {
                m_WaveDissolveSquare->GetRenderObject().GetMaterial()->SetFloat("u_Time", time);
            }

            BeginScene();
            
            if (ImGuiEnabled && m_TexturedSquare) {
                m_ImGuiOverlay->OnRender(m_TexturedSquare->GetRenderObject(), m_ShowFPSCounter, 
                    m_CurrentFPS, m_FPS, m_FrameTime, m_FPS1PercentLow, m_FPS1PercentHigh);
                m_ImGuiOverlay->RenderTransformControls(m_TexturedSquare->GetRenderObject());
                m_ImGuiOverlay->RenderProfiler();
                m_ImGuiOverlay->RenderRendererSettings();
            }

            SceneManager::Get().Render(m_Renderer);
            EndScene();
            Present();
        }
    }

    void Application::UpdateFPSCounter(float deltaTime, float currentTime) {
        m_FrameTime = deltaTime;
        m_CurrentFPS = 1.0f / m_FrameTime;
        m_FPSSamples[m_CurrentFPSSample] = m_CurrentFPS;
        m_CurrentFPSSample = (m_CurrentFPSSample + 1) % FPS_SAMPLE_COUNT;

        m_FPSUpdateTimer += deltaTime;
        if (m_FPSUpdateTimer >= 0.5f) {
            // Calculate average FPS
            float sum = 0.0f;
            for (float fps : m_FPSSamples) {
                sum += fps;
            }
            m_FPS = sum / FPS_SAMPLE_COUNT;

            // Calculate 1% highs and lows
            std::vector<float> sortedFPS = m_FPSSamples;
            std::sort(sortedFPS.begin(), sortedFPS.end());
            
            size_t onePercent = FPS_SAMPLE_COUNT / 100;
            if (onePercent < 1) onePercent = 1;

            // Calculate 1% lows (average of bottom 1%)
            float lowSum = 0.0f;
            for (size_t i = 0; i < onePercent; i++) {
                lowSum += sortedFPS[i];
            }
            m_FPS1PercentLow = lowSum / onePercent;

            // Calculate 1% highs (average of top 1%)
            float highSum = 0.0f;
            for (size_t i = 0; i < onePercent; i++) {
                highSum += sortedFPS[FPS_SAMPLE_COUNT - 1 - i];
            }
            m_FPS1PercentHigh = highSum / onePercent;

            m_FPSUpdateTimer = 0.0f;
        }
    }

    void Application::InitWindow(const char* title, int width, int height) {
        LOG_INFO("Initializing window: {0} ({1}x{2})", title, width, height);
        WindowProps props(title, width, height);
        m_Window = std::unique_ptr<Window>(Window::Create(props));
        
        // Set event callback
        m_Window->SetEventCallback([this](Event& e) {
            EventDispatcher dispatcher(e);
            
            dispatcher.Dispatch<WindowCloseEvent>(
                std::function<bool(WindowCloseEvent&)>([this](WindowCloseEvent& e) {
                    LOG_INFO("Window Close Event received");
                    m_Running = false;
                    return true;
                })
            );

            // Forward events to InputSystem
            if (!e.IsHandled()) {
                m_InputSystem->OnEvent(e);
            }
        });
    }

    void Application::ShutdownWindow() {
        LOG_INFO("Shutting down window");
        m_Window.reset();
    }

    void Application::BeginScene() {
        m_Window->SetClear(0.1f, 0.1f, 0.1f, 1.0f);
        
        m_TerrainSystem->Render(m_Renderer);
        
        if (m_Triangle) m_Triangle->OnRender(m_Renderer);
        if (m_TexturedSquare) m_TexturedSquare->OnRender(m_Renderer);
        if (m_TransparentSquare) m_TransparentSquare->OnRender(m_Renderer);
        if (m_FileShaderSquare) m_FileShaderSquare->OnRender(m_Renderer);
        if (m_PixelatedSquare) m_PixelatedSquare->OnRender(m_Renderer);
        if (m_WaveDissolveSquare) m_WaveDissolveSquare->OnRender(m_Renderer);
        if (m_BlurSquare) m_BlurSquare->OnRender(m_Renderer);

        m_Renderer.Draw();
        m_ImGuiLayer->Begin();
    }

    void Application::EndScene() {
        m_ImGuiLayer->End();
    }

    void Application::Present() {
        m_Window->OnUpdate();
    }

    void Application::SetViewport(int x, int y, int width, int height) {
        LOG_TRACE_CONCAT("Set Viewport: ", x, ",", y, ",", width, ",", height);
    }

    void Application::CreateFileShaderSquare() {
        m_FileShaderSquare = std::make_unique<RenderableObject>();
        auto va = std::shared_ptr<VertexArray>(VertexArray::Create());
        
        std::shared_ptr<VertexBuffer> squareVB(
            VertexBuffer::Create(MeshTemplates::TexturedSquare.data(), 
            MeshTemplates::TexturedSquare.size() * sizeof(float)));

        BufferLayout squareLayout = {
            { ShaderDataType::Float3, "aPosition" },
            { ShaderDataType::Float2, "aTexCoord" }
        };
        
        squareVB->SetLayout(squareLayout);
        va->AddVertexBuffer(squareVB);

        std::shared_ptr<IndexBuffer> squareIB(
            IndexBuffer::Create(MeshTemplates::SquareIndices.data(), 
            MeshTemplates::SquareIndices.size()));
        va->SetIndexBuffer(squareIB);

        // Load shader from files
        auto shader = std::shared_ptr<Shader>(
            Shader::CreateFromFiles(
                "assets/shaders/basic.vert", 
                "assets/shaders/basic.frag"
            )
        );

        auto material = std::make_shared<Material>(shader);
        material->SetVector4("u_Color", glm::vec4(0.2f, 0.8f, 0.3f, 1.0f));

        Transform transform;
        transform.position = glm::vec3(-0.5f, 0.5f, 0.5f);
        transform.scale = glm::vec3(0.5f);

        m_FileShaderSquare->SetRenderObject(std::make_unique<RenderObject>(va, material, transform));
    }

    void Application::CreatePixelatedSquare() {
        m_PixelatedSquare = std::make_unique<RenderableObject>();
        
        auto shader = DefaultShaders::LoadPixelShader();
        auto material = std::make_shared<Material>(shader);
        
        material->SetFloat("u_PixelSize", 8.0f);
        material->SetTexture("u_Texture", m_TestTexture);
        material->SetVector4("u_Color", glm::vec4(1.0f));
        
        Transform transform;
        transform.position = glm::vec3(1.5f, 0.5f, 0.5f);
        transform.scale = glm::vec3(0.5f);

        // Use the same vertex array as textured square
        auto va = m_TexturedSquare ? m_TexturedSquare->GetRenderObject().GetVertexArray() : nullptr;
        m_PixelatedSquare->SetRenderObject(std::make_unique<RenderObject>(va, material, transform));
    }

    void Application::CreateWaveDissolveSquare() {
        m_WaveDissolveSquare = std::make_unique<RenderableObject>();
        
        auto shader = DefaultShaders::LoadWaveDissolveShader();
        auto material = std::make_shared<Material>(shader);
        
        material->SetTexture("u_Texture", m_TestTexture);
        material->SetVector4("u_Color", glm::vec4(1.0f));
        material->SetFloat("u_WaveSpeed", 2.0f);
        material->SetFloat("u_WaveFrequency", 10.0f);
        material->SetFloat("u_DissolveAmount", 0.5f);
        
        Transform transform;
        transform.position = glm::vec3(-1.5f, -0.5f, 0.5f);
        transform.scale = glm::vec3(0.5f);

        // Use the same vertex array as textured square
        auto va = m_TexturedSquare ? m_TexturedSquare->GetRenderObject().GetVertexArray() : nullptr;
        m_WaveDissolveSquare->SetRenderObject(std::make_unique<RenderObject>(va, material, transform));
    }

    void Application::CreateBlurSquare() {
        m_BlurSquare = std::make_unique<RenderableObject>();
        
        auto shader = DefaultShaders::LoadBlurShader();
        auto material = std::make_shared<Material>(shader);
        
        material->SetTexture("u_Texture", m_TestTexture);
        material->SetVector4("u_Color", glm::vec4(1.0f));
        material->SetFloat("u_BlurStrength", 0.005f);
        
        Transform transform;
        transform.position = glm::vec3(0.0f, -0.5f, 0.5f);
        transform.scale = glm::vec3(0.5f);
        // Use the same vertex array as textured square
        auto va = m_TexturedSquare ? m_TexturedSquare->GetRenderObject().GetVertexArray() : nullptr;
        m_BlurSquare->SetRenderObject(std::make_unique<RenderObject>(va, material, transform));
    }
}