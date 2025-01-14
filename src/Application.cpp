#include <pch.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Application.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"
#include "Events/KeyCodes.h"
#include "Events/EventDispatcher.h"
#include "Camera/OrthographicCamera.h"
#include "Renderer/VertexArray.h"
#include <imgui.h>
#include "Shader/DefaultShaders.h"
#include "Renderer/MeshTemplates.h"
#include "Debug/Profiler.h"
#include "Scene/SceneManager.h"
#include "UI/ImGuiOverlay.h"
#include "Core/AssetManager.h"

namespace Engine {
    /**
     * @brief Initialize the application and all subsystems
     */
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
        m_TerrainSystem->Initialize(m_Renderer);

        // Preload frequently used assets
        AssetManager::Get().PreloadFrequentAssets();

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

        m_TestTexture = AssetManager::Get().LoadResource<Texture>("assets/textures/test.png");

        if (m_TestTexture) {
            AssetManager::Get().MarkAsFrequentlyUsed<Texture>("assets/textures/test.png");
        }

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

        m_RenderableObjects.push_back(createTexturedSquare(glm::vec3(0.5f), glm::vec4(1.0f)));
        m_RenderableObjects.push_back(createTexturedSquare(glm::vec3(-0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)));

        
        // TODO: use async when loading see example
        /*Example
        auto futureTexture = AssetManager::Get().LoadResourceAsync<Texture>("assets/textures/large.png");
    
        m_LargeTexture = futureTexture.get(); 
        */
        InitializeToggleStates();
    }

    /**
     * @brief Clean up application resources
     */
    Application::~Application() {
        if (m_ImGuiLayer) {
            m_ImGuiLayer->Shutdown();
        }
        LOG_INFO("Application Destroyed");

        // Resources automatically unloaded when reference count hits 0
        m_TestTexture = nullptr;
        
        AssetManager::Get().UnloadUnused();
    }

    /**
     * @brief Main application loop
     * @details Handles rendering, input processing, and event management
     */
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

            EventDebugger::Get().UpdateTimestamps(deltaTime);

            ProcessEvents();

            m_TerrainSystem->Update(deltaTime);
            m_InputSystem->Update(deltaTime);
            SceneManager::Get().Update(deltaTime);

            if (HandleKeyToggle(GLFW_KEY_F3, time)) {
                m_ShowFPSCounter = m_KeyToggles[GLFW_KEY_F3].currentValue;
            }

            if (HandleKeyToggle(GLFW_KEY_F2, time)) {
                m_ImGuiEnabled = m_KeyToggles[GLFW_KEY_F2].currentValue;
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
            
            // Render scene objects
            if (m_Triangle) m_Triangle->OnRender(m_Renderer);
            if (m_TexturedSquare) m_TexturedSquare->OnRender(m_Renderer);
            if (m_TransparentSquare) m_TransparentSquare->OnRender(m_Renderer);
            if (m_FileShaderSquare) m_FileShaderSquare->OnRender(m_Renderer);
            if (m_PixelatedSquare) m_PixelatedSquare->OnRender(m_Renderer);
            if (m_WaveDissolveSquare) m_WaveDissolveSquare->OnRender(m_Renderer);
            if (m_BlurSquare) m_BlurSquare->OnRender(m_Renderer);
            
            EndScene();
        }
    }

    /**
     * @brief Process pending events in the event queue
     */
    void Application::ProcessEvents() {
        static float eventTimer = 0.0f;
        eventTimer += 0.016f; // Approximate for 60fps
        
        if (eventTimer >= 1.0f) { // Create a test event every second
            auto testEvent = std::make_shared<KeyPressedEvent>(GLFW_KEY_T);
            testEvent->SetPriority(EventPriority::Normal);
            EventQueue::Get().PushEvent(testEvent);
            eventTimer = 0.0f;
        }

        std::shared_ptr<Event> event;
        while (EventQueue::Get().PopEvent(event)) {
            EventDispatcher dispatcher(*event.get());
            
            dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) -> bool {
                LOG_INFO("Window Close Event received");
                m_Running = false;
                return true;
            });

            if (!event->IsHandled()) {
                m_InputSystem->OnEvent(*event);
            }
        }

        // Add chunk range control with number keys
        if (auto* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow())) {
            for (int i = 0; i <= 9; i++) {
                if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS) {
                    m_TerrainSystem->SetChunkRange(i);
                    break;
                }
            }
        }
    }

    /**
     * @brief Update FPS counter and related statistics
     * @param deltaTime Time since last frame
     * @param currentTime Current application time
     */
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

    /**
     * @brief Initialize window with specified parameters
     * @param title Window title
     * @param width Window width
     * @param height Window height
     */
    void Application::InitWindow(const char* title, int width, int height) {
        LOG_INFO("Initializing window: {0} ({1}x{2})", title, width, height);
        WindowProps props(title, width, height);
        m_Window = std::unique_ptr<Window>(Window::Create(props));
        
        m_Window->SetEventCallback([this](Event& e) -> bool {
            std::shared_ptr<Event> eventPtr = nullptr;
            
            if (e.GetEventType() == EventType::WindowResize) {
                auto& resizeEvent = static_cast<WindowResizeEvent&>(e);
                SetViewport(0, 0, resizeEvent.GetWidth(), resizeEvent.GetHeight());
            }
            
            // Clone the event based on its type
            if (e.GetEventType() == EventType::KeyPressed) {
                auto& keyEvent = static_cast<KeyPressedEvent&>(e);
                eventPtr = std::make_shared<KeyPressedEvent>(keyEvent.GetKeyCode(), keyEvent.IsRepeat());
            }
            else if (e.GetEventType() == EventType::MouseMoved) {
                auto& mouseEvent = static_cast<MouseMovedEvent&>(e);
                eventPtr = std::make_shared<MouseMovedEvent>(mouseEvent.GetX(), mouseEvent.GetY());
            }
            // TODO: add more event types
            
            if (eventPtr) {
                EventQueue::Get().PushEvent(eventPtr);
            }
            return false;
        });
    }

    void Application::ShutdownWindow() {
        LOG_INFO("Shutting down window");
        m_Window.reset();
    }

    void Application::BeginScene() {
        m_Window->SetClear(0.1f, 0.1f, 0.1f, 1.0f);
        
        m_TerrainSystem->Render(m_Renderer);
        m_Renderer.Draw();
        
        m_ImGuiLayer->Begin();
        
        if (m_ImGuiEnabled && m_TexturedSquare) {
            m_ImGuiOverlay->OnRender(m_TexturedSquare->GetRenderObject(), m_ShowFPSCounter, 
                m_CurrentFPS, m_FPS, m_FrameTime, m_FPS1PercentLow, m_FPS1PercentHigh);
            m_ImGuiOverlay->RenderTransformControls(m_TexturedSquare->GetRenderObject());
            m_ImGuiOverlay->RenderProfiler();
            m_ImGuiOverlay->RenderRendererSettings();
            m_ImGuiOverlay->RenderEventDebugger();
            m_ImGuiOverlay->RenderTerrainControls(*m_TerrainSystem);
        }
    }

    void Application::EndScene() {
        m_ImGuiLayer->End();
        m_Window->OnUpdate();
    }

    void Application::Present() {
        
    }

    void Application::SetViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
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

    void Application::InitializeToggleStates() {
        AddToggleState(GLFW_KEY_F2, true);  // ImGui enabled by default
        AddToggleState(GLFW_KEY_F3, true);  // FPS counter enabled by default
    }

    bool Application::HandleKeyToggle(int key, float currentTime) {
        if (auto* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow())) {
            auto& state = m_KeyToggles[key];
            bool currentState = glfwGetKey(window, key) == GLFW_PRESS;
            
            // Track when key is first pressed
            if (currentState && !state.previousState) {
                state.pressStartTime = currentTime;
            }
            
            // Check for toggle condition on release
            if (state.previousState && !currentState) {
                bool shouldToggle = (currentTime - state.pressStartTime) < MAX_TOGGLE_HOLD_TIME;
                state.previousState = currentState;
                if (shouldToggle) {
                    state.currentValue = !state.currentValue;
                    return true;
                }
            }
            
            state.previousState = currentState;
        }
        return false;
    }

    void Application::AddToggleState(int key, bool defaultValue) {
        m_KeyToggles[key] = KeyToggleState{false, 0.0f, defaultValue};
    }

    void Application::RemoveToggleState(int key) {
        m_KeyToggles.erase(key);
    }

    bool Application::GetToggleState(int key) const {
        auto it = m_KeyToggles.find(key);
        return it != m_KeyToggles.end() ? it->second.currentValue : false;
    }

    void Application::SetToggleState(int key, bool value) {
        auto it = m_KeyToggles.find(key);
        if (it != m_KeyToggles.end()) {
            it->second.currentValue = value;
        }
    }
}