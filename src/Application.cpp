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

        // Create shader for triangle
        m_Shader = DefaultShaders::LoadSimpleColorShader();
        m_Material = std::make_shared<Material>(m_Shader);
        m_Material->SetVector4("u_Color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));

        // Create triangle mesh using template
        auto triangleVertices = MeshTemplates::GetVertexData(MeshTemplates::Triangle);
        m_VertexArray.reset(VertexArray::Create());
        
        std::shared_ptr<VertexBuffer> vertexBuffer(
            VertexBuffer::Create(triangleVertices.data(), 
            triangleVertices.size() * sizeof(float)));
        
        BufferLayout layout = {
            { ShaderDataType::Float3, "aPosition" }
        };
        
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        std::shared_ptr<IndexBuffer> indexBuffer(
            IndexBuffer::Create(MeshTemplates::TriangleIndices.data(), 
            MeshTemplates::TriangleIndices.size()));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        // Create textured square
        auto squareVertices = MeshTemplates::TexturedSquare;
        m_SquareVA.reset(VertexArray::Create());
        
        std::shared_ptr<VertexBuffer> squareVB(
            VertexBuffer::Create(squareVertices.data(), 
            squareVertices.size() * sizeof(float)));

        BufferLayout squareLayout = {
            { ShaderDataType::Float3, "aPosition" },
            { ShaderDataType::Float2, "aTexCoord" }
        };
        
        squareVB->SetLayout(squareLayout);
        m_SquareVA->AddVertexBuffer(squareVB);

        std::shared_ptr<IndexBuffer> squareIB(
            IndexBuffer::Create(MeshTemplates::SquareIndices.data(), 
            MeshTemplates::SquareIndices.size()));
        m_SquareVA->SetIndexBuffer(squareIB);

        // Load texture
        m_TestTexture = Texture::Create("assets/textures/test.png");

        // Create shader for textured square
        m_SquareShader = DefaultShaders::LoadTexturedShader();
        m_SquareMaterial = std::make_shared<Material>(m_SquareShader);
        m_SquareMaterial->SetTexture("u_Texture", m_TestTexture);
        m_SquareMaterial->SetVector4("u_Color", glm::vec4(1.0f));

        // Create transform for square
        m_SquareTransform.position = glm::vec3(0.5f, 0.5f, 0.5f);
        // m_SquareTransform.rotation = glm::vec3(0.0f); // In radians
        m_SquareTransform.scale = glm::vec3(0.5f);

        // Transparent square
        m_TransparentSquareVA = m_SquareVA; // Reuse the square geometry
        m_TransparentShader = DefaultShaders::LoadTexturedShader();
        m_TransparentMaterial = std::make_shared<Material>(m_TransparentShader);
        m_TransparentMaterial->SetVector4("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)); // Semi-transparent red
        
        // Position transparent square slightly in front of the textured square
        m_TransparentTransform = m_SquareTransform;
        m_TransparentTransform.position.z -= 0.1f;

        CreateFileShaderSquare();
        CreatePixelatedSquare();
        CreateWaveDissolveSquare();
        CreateBlurSquare();
        
        m_FPSSamples.resize(FPS_SAMPLE_COUNT, 0.0f);

        // Initialize terrain system
        m_TerrainSystem = std::make_unique<TerrainSystem>();

        m_InputSystem = std::make_unique<InputSystem>(m_Window.get(), m_Renderer);

        m_ImGuiOverlay = std::make_unique<ImGuiOverlay>(m_Window.get());
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

            // Update transform (TRS)
            // m_SquareTransform.rotation.z = time; // Rotate around Z axis
            // float scale = (sin(time) + 2.0f) * 0.5f; // Scale between 0.5 and 1.5
            // m_SquareTransform.scale = glm::vec3(scale);
            m_SquareTransform.scale = glm::vec3(1.0f); // Or whatever fixed scale you want

            // Add WASD movement for the square
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_A) == GLFW_PRESS)
                m_SquareTransform.position.x -= 2.0f * deltaTime;
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_D) == GLFW_PRESS)
                m_SquareTransform.position.x += 2.0f * deltaTime;
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_W) == GLFW_PRESS)
                m_SquareTransform.position.y += 2.0f * deltaTime;
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_S) == GLFW_PRESS)
                m_SquareTransform.position.y -= 2.0f * deltaTime;

            // Add manual rotation controls with Q and E keys
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_Q) == GLFW_PRESS)
                m_SquareTransform.rotation.z += 2.0f * deltaTime;
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_E) == GLFW_PRESS)
                m_SquareTransform.rotation.z -= 2.0f * deltaTime;

            // Update wave dissolve effect
            m_WaveDissolveMaterial->SetFloat("u_Time", time);

            BeginScene();
            
            if (ImGuiEnabled) {
                m_ImGuiOverlay->OnRender(m_SquareTransform, m_ShowFPSCounter, 
                    m_CurrentFPS, m_FPS, m_FrameTime, m_FPS1PercentLow, m_FPS1PercentHigh);
                m_ImGuiOverlay->RenderTransformControls(m_SquareTransform);
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
        //* OPTIONAL: make magenta to make it clear when clear is rendered
        // RGB Alpha
        m_Window->SetClear(1.0f, 0.0f, 1.0f, 0.0f);
        // m_Window->SetClear(0.1f, 0.1f, 0.1f, 1.0f); // Dark Grey
        
        m_Renderer.Submit(m_VertexArray, m_Material);
        m_Renderer.Submit(m_SquareVA, m_SquareMaterial, m_SquareTransform);
        m_Renderer.Submit(m_TransparentSquareVA, m_TransparentMaterial, m_TransparentTransform);
        m_Renderer.Submit(m_FileShaderSquareVA, m_FileShaderSquareMaterial, m_FileShaderSquareTransform);
        m_Renderer.Submit(m_PixelatedSquareVA, m_PixelatedMaterial, m_PixelatedTransform);
        m_Renderer.Submit(m_WaveDissolveVA, m_WaveDissolveMaterial, m_WaveDissolveTransform);
        m_Renderer.Submit(m_BlurVA, m_BlurMaterial, m_BlurTransform);
        
        m_TerrainSystem->Render(m_Renderer);

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
        // Create vertex array using existing square mesh
        m_FileShaderSquareVA.reset(VertexArray::Create());
        
        std::shared_ptr<VertexBuffer> squareVB(
            VertexBuffer::Create(MeshTemplates::TexturedSquare.data(), 
            MeshTemplates::TexturedSquare.size() * sizeof(float)));

        BufferLayout squareLayout = {
            { ShaderDataType::Float3, "aPosition" },
            { ShaderDataType::Float2, "aTexCoord" }
        };
        
        squareVB->SetLayout(squareLayout);
        m_FileShaderSquareVA->AddVertexBuffer(squareVB);

        std::shared_ptr<IndexBuffer> squareIB(
            IndexBuffer::Create(MeshTemplates::SquareIndices.data(), 
            MeshTemplates::SquareIndices.size()));
        m_FileShaderSquareVA->SetIndexBuffer(squareIB);

        // Load shader from files
        m_FileShaderSquareShader = std::shared_ptr<Shader>(
            Shader::CreateFromFiles(
                "assets/shaders/basic.vert", 
                "assets/shaders/basic.frag"
            )
        );

        m_FileShaderSquareMaterial = std::make_shared<Material>(m_FileShaderSquareShader);
        m_FileShaderSquareMaterial->SetVector4("u_Color", glm::vec4(0.2f, 0.8f, 0.3f, 1.0f));

        // Position the square to the left of the original square
        m_FileShaderSquareTransform.position = glm::vec3(-0.5f, 0.5f, 0.5f);
        m_FileShaderSquareTransform.scale = glm::vec3(0.5f);
    }

    void Application::CreatePixelatedSquare() {
        m_PixelatedSquareVA = m_SquareVA;
        
        m_PixelatedShader = DefaultShaders::LoadPixelShader();
        m_PixelatedMaterial = std::make_shared<Material>(m_PixelatedShader);
        
        m_PixelatedMaterial->SetFloat("u_PixelSize", 8.0f);
        m_PixelatedMaterial->SetTexture("u_Texture", m_TestTexture);
        m_PixelatedMaterial->SetVector4("u_Color", glm::vec4(1.0f));
        
        m_PixelatedTransform.position = glm::vec3(1.5f, 0.5f, 0.5f);
        m_PixelatedTransform.scale = glm::vec3(0.5f);
    }

    void Application::CreateWaveDissolveSquare() {
        m_WaveDissolveVA = m_SquareVA; // Reuse square mesh
        m_WaveDissolveShader = DefaultShaders::LoadWaveDissolveShader();
        m_WaveDissolveMaterial = std::make_shared<Material>(m_WaveDissolveShader);
        
        m_WaveDissolveMaterial->SetTexture("u_Texture", m_TestTexture);
        m_WaveDissolveMaterial->SetVector4("u_Color", glm::vec4(1.0f));
        m_WaveDissolveMaterial->SetFloat("u_WaveSpeed", 2.0f);
        m_WaveDissolveMaterial->SetFloat("u_WaveFrequency", 10.0f);
        m_WaveDissolveMaterial->SetFloat("u_DissolveAmount", 0.5f);
        
        m_WaveDissolveTransform.position = glm::vec3(-1.5f, -0.5f, 0.5f);
        m_WaveDissolveTransform.scale = glm::vec3(0.5f);
    }

    void Application::CreateBlurSquare() {
        m_BlurVA = m_SquareVA; // Reuse square mesh
        m_BlurShader = DefaultShaders::LoadBlurShader();
        m_BlurMaterial = std::make_shared<Material>(m_BlurShader);
        
        m_BlurMaterial->SetTexture("u_Texture", m_TestTexture);
        m_BlurMaterial->SetVector4("u_Color", glm::vec4(1.0f));
        m_BlurMaterial->SetFloat("u_BlurStrength", 0.005f);
        
        m_BlurTransform.position = glm::vec3(0.0f, -0.5f, 0.5f);
        m_BlurTransform.scale = glm::vec3(0.5f);
    }
}