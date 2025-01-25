#include <pch.h>
#include "Application.h"

#include <imgui.h>

// Core headers
#include "Application.h"
#include "Core/AssetManager.h"

// Event system
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"
#include "Events/KeyCodes.h"
#include "Events/EventDispatcher.h"

// Systems
#include "Camera/OrthographicCamera.h"
#include "Scene/SceneManager.h"
#include "UI/ImGuiOverlay.h"

// Shader system
#include "Shader/DefaultShaders.h"
#include "Shader/ShaderHotReload.h"

/**
 * @namespace Engine
 * @brief Core engine namespace containing the main application and system components
 *
 * The Engine namespace encapsulates all core functionality of the voxel engine including:
 * - Application lifecycle management
 * - Window handling and event processing
 * - Rendering systems and ImGui integration
 * - Input processing
 * - Terrain generation and management
 * - Asset management
 * - Performance profiling and debugging tools
 *
 * Key components:
 * - Application: Main application class managing the game loop and subsystems
 * - TerrainSystem: Handles voxel terrain generation and rendering
 * - InputSystem: Processes user input and dispatches events
 * - ImGuiLayer: Handles debug UI rendering
 * - Renderer: Manages graphics rendering
 * 
 * The engine uses a component-based architecture with systems for:
 * - Event handling through EventQueue
 * - Asset management via AssetManager
 * - Performance profiling with Profiler
 * - Scene management through SceneManager
 *
 * @note All engine components should be initialized through the Application class
 */
namespace Engine {
    constexpr float EVENT_PROCESS_RATE = 60.0f;
    constexpr float EVENT_PROCESS_INTERVAL = 1.0f / EVENT_PROCESS_RATE;

    Application* Application::s_Instance = nullptr;

    /**
     * @brief Initialize the application and all subsystems
     */
    Application::Application() 
    {
        s_Instance = this;
        LOG_INFO("Creating Application");
        
        Engine::Profiler::Get().BeginSession("Runtime");

        InitWindow("Voxel Engine", 1280, 720);

        // Initialize renderer before other systems
        m_Renderer = std::make_unique<Renderer>();
        m_Renderer->Initialize();

        // Create and initialize ImGui layer first
        m_ImGuiLayer = std::make_unique<ImGuiLayer>(m_Window.get());
        m_ImGuiLayer->Init(m_Window.get());  // Explicitly call Init
        LOG_INFO("ImGui initialized");
        
        // Initialize script system first
        m_ScriptSystem = std::make_unique<LuaScriptSystem>();
        if (!m_ScriptSystem) {
            LOG_ERROR("Failed to create script system");
            return;
        }
        
        try {
            m_ScriptSystem->Initialize();
        } catch (const std::exception& e) {
            LOG_ERROR_CONCAT("Failed to initialize script system: ", e.what());
            return;
        }
        
        // Initialize remaining systems after script system is ready
        AssetManager::Get().PreloadFrequentAssets();
        m_TerrainSystem = std::make_unique<TerrainSystem>();
        m_InputSystem = std::make_unique<InputSystem>(m_Window.get(), *m_Renderer);
        m_ImGuiOverlay = std::make_unique<ImGuiOverlay>(m_Window.get());
        
        InitializeToggleStates();
        DefaultShaders::PreloadShaders();
    }

    /**
     * @brief Clean up application resources
     */
    Application::~Application() {
        if (m_ImGuiLayer) {
            m_ImGuiLayer->Shutdown();
        }
        LOG_INFO("Application Destroyed");
        
        AssetManager::Get().UnloadUnused();
    }

    /**
     * @brief Main application loop
     * @details Handles rendering, input processing, and event management
     */
    void Application::Run() 
    {
        PROFILE_FUNCTION();
        LOG_INFO("Application Starting...");
        
        Profiler::Get().BeginSession("Runtime");
        
        float lastFrameTime = 0.0f;
        
        while (m_Running && m_Window) {
            auto time = static_cast<float>(glfwGetTime());
            float deltaTime = time - lastFrameTime;
            lastFrameTime = time;
            
            // Update systems
            EventDebugger::Get().UpdateTimestamps(deltaTime);
            ProcessEvents();
            m_TerrainSystem->Update(deltaTime);
            m_InputSystem->Update(deltaTime);
            SceneManager::Get().Update(deltaTime);
            
            // Handle toggles
            if (HandleKeyToggle(GLFW_KEY_F3, time)) {
                m_ShowFPSCounter = m_KeyToggles[GLFW_KEY_F3].currentValue;
            }
            if (HandleKeyToggle(GLFW_KEY_F2, time)) {
                m_ImGuiEnabled = m_KeyToggles[GLFW_KEY_F2].currentValue;
            }
            
            if (m_ShowFPSCounter) { UpdateFPSCounter(deltaTime); };
            
            #ifdef ENGINE_DEBUG
            // Update hot-reloading system
            ShaderHotReload::Get().Update();
            #endif
            
            // Render frame
            BeginScene();
            OnImGuiRender();
            Present();
            EndScene();
        }
    }

    /**
     * @brief Process pending events in the event queue
     */
    void Application::ProcessEvents() {
        static float eventTimer = 0.0f;
        static const float fixedDeltaTime = 1.0f / 60.0f;
        eventTimer += fixedDeltaTime;

        // Process events at fixed 60Hz rate
        if (eventTimer >= EVENT_PROCESS_INTERVAL) {
            eventTimer = 0.0f;
            
            EventQueue::Get().ProcessEvents([this](std::shared_ptr<Event> event) {
                EventDispatcher dispatcher(*event.get());
                
                dispatcher.Dispatch<WindowCloseEvent>([this](const WindowCloseEvent& e [[maybe_unused]]) {
                    LOG_INFO("Window Close Event received");
                    m_Running = false;
                    return true;
                });

                if (!event->IsHandled()) {
                    m_InputSystem->OnEvent(*event);
                }
                return true;
            });

            // Replace GLFW key checking with input system
            for (int i = 0; i <= 9; i++) {
                if (m_InputSystem->IsKeyPressed(KeyCode::NUM_0 + i)) {
                    auto event = std::make_shared<KeyPressedEvent>(KeyCode::NUM_0 + i);
                    EventQueue::Get().PushEvent(event);
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
    void Application::UpdateFPSCounter(float deltaTime) {
        m_FPSCounter.Update(deltaTime);
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
        
        m_Window->SetEventCallback([this](const Event& e) {
            std::shared_ptr<Event> eventPtr = nullptr;
            
            if (e.GetEventType() == EventType::WindowResize) {
                const auto* resizeEvent = static_cast<const WindowResizeEvent*>(&e);
                SetViewport(0, 0, resizeEvent->GetWidth(), resizeEvent->GetHeight());
            }
            
            // Clone the event based on its type
            if (e.GetEventType() == EventType::KeyPressed) {
                const auto* keyEvent = static_cast<const KeyPressedEvent*>(&e);
                eventPtr = std::make_shared<KeyPressedEvent>(keyEvent->GetKeyCode(), keyEvent->IsRepeat());
            }
            else if (e.GetEventType() == EventType::MouseMoved) {
                const auto* mouseEvent = static_cast<const MouseMovedEvent*>(&e);
                eventPtr = std::make_shared<MouseMovedEvent>(mouseEvent->GetX(), mouseEvent->GetY());
            }
            
            if (eventPtr) {
                EventQueue::Get().PushEvent(eventPtr);
            }
            return false;
        });
        m_Window->SetContext();
    }

    void Application::ShutdownWindow() {
        LOG_INFO("Shutting down window");
        m_Window.reset();
    }

    void Application::BeginScene() {
        PROFILE_FUNCTION();

        m_Renderer->Clear({0.1f, 0.1f, 0.1f, 1.0f});
        
        // Only render terrain if it exists
        if (m_TerrainSystem) {
            m_TerrainSystem->Render(*m_Renderer);
        }
        
        m_ImGuiLayer->Begin();
        
        if (m_ImGuiEnabled) {
            static RenderObject dummyRenderObject;
            m_ImGuiOverlay->OnRender(dummyRenderObject, m_ShowFPSCounter, 
                m_FPSCounter.GetCurrentFPS(), 
                m_FPSCounter.GetAverageFPS(), 
                m_FPSCounter.GetFrameTime(), 
                m_FPSCounter.GetOnePercentLow(), 
                m_FPSCounter.GetOnePercentHigh());
            m_ImGuiOverlay->RenderProfiler();
            m_ImGuiOverlay->RenderRendererSettings();
            m_ImGuiOverlay->RenderEventDebugger();
            if (m_TerrainSystem) {
                m_ImGuiOverlay->RenderTerrainControls(*m_TerrainSystem);
            }
        }
    }

    void Application::Present() {
        PROFILE_FUNCTION();
        
        m_Renderer->Draw();
    }

    void Application::EndScene() {
        PROFILE_FUNCTION();

        m_ImGuiLayer->End();
        m_Window->OnUpdate();
    }

    void Application::InitializeToggleStates() {
        AddToggleState(GLFW_KEY_F2, true);  // ImGui enabled by default
        AddToggleState(GLFW_KEY_F3, true);  // FPS counter enabled by default
    }

    bool Application::HandleKeyToggle(int key, float currentTime) {
        auto& state = m_KeyToggles[key];
        bool currentState = m_InputSystem->IsKeyPressed(key);
        
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