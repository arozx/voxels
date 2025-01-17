#include <pch.h>

// External dependencies
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
    /**
     * @brief Initialize the application and all subsystems
     */
    Application::Application() 
    {
        LOG_INFO("Creating Application");
        
        InitWindow("Voxel Engine", 1280, 720);
        if (!m_Window) {
            LOG_ERROR("Failed to create window!");
            m_Running = false;
            return;
        }
        
        m_Window->SetContext();
        
        // Initialize core systems
        m_ImGuiLayer = std::make_unique<ImGuiLayer>();
        m_ImGuiLayer->Init(m_Window.get());
        
        m_Renderer.Init();
        
        // Initialize subsystems
        m_TerrainSystem = std::make_unique<TerrainSystem>();
        m_TerrainSystem->Initialize(m_Renderer);
        
        AssetManager::Get().PreloadFrequentAssets();
        
        m_InputSystem = std::make_unique<InputSystem>(m_Window.get(), m_Renderer);
        m_ImGuiOverlay = std::make_unique<ImGuiOverlay>(m_Window.get());
        
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
            float time = static_cast<float>(glfwGetTime());
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
            
            UpdateFPSCounter(deltaTime, time);
            
            // Render frame
            BeginScene();
            m_TerrainSystem->Render(m_Renderer);
            m_Renderer.Draw();
            EndScene();
        }
    }

    /**
     * @brief Process pending events in the event queue
     */
    void Application::ProcessEvents() {
        static float eventTimer = 0.0f;
        // TODO: Sync to FPS
        eventTimer += 0.016f; // Approximate for 60fps
        
        if (eventTimer >= 1.0f) { // Create a test event every second
            auto testEvent = std::make_shared<KeyPressedEvent>(GLFW_KEY_T);
            testEvent->SetPriority(EventPriority::Normal);
            EventQueue::Get().PushEvent(testEvent);
            eventTimer = 0.0f;
        }

        EventQueue::Get().ProcessEvents([this](std::shared_ptr<Event> event) {
            EventDispatcher dispatcher(*event.get());
            
            dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) -> bool {
                LOG_INFO("Window Close Event received");
                m_Running = false;
                return true;
            });

            if (!event->IsHandled()) {
                m_InputSystem->OnEvent(*event);
            }
            return true;
        });

        // Add chunk range control with number keys
        if (auto* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow())) {
            for (int i = 0; i <= 9; i++) {
                if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS) {
                    auto event = std::make_shared<KeyPressedEvent>(GLFW_KEY_0 + i);
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
    void Application::UpdateFPSCounter(float deltaTime, float currentTime) {
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
        PROFILE_FUNCTION();

        m_Window->SetClear(0.1f, 0.1f, 0.1f, 1.0f);
        
        m_TerrainSystem->Render(m_Renderer);
        m_Renderer.Draw();
        
        m_ImGuiLayer->Begin();
        
        if (m_ImGuiEnabled) {
            static RenderObject dummyRenderObject;  // Create a static dummy object
            m_ImGuiOverlay->OnRender(dummyRenderObject, m_ShowFPSCounter, 
                m_FPSCounter.GetCurrentFPS(), 
                m_FPSCounter.GetAverageFPS(), 
                m_FPSCounter.GetFrameTime(), 
                m_FPSCounter.GetOnePercentLow(), 
                m_FPSCounter.GetOnePercentHigh());
            m_ImGuiOverlay->RenderProfiler();
            m_ImGuiOverlay->RenderRendererSettings();
            m_ImGuiOverlay->RenderEventDebugger();
            m_ImGuiOverlay->RenderTerrainControls(*m_TerrainSystem);
        }
    }

    void Application::EndScene() {
        PROFILE_FUNCTION();

        m_ImGuiLayer->End();
        m_Window->OnUpdate();
    }

    void Application::Present() {
        
    }

    void Application::SetViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
        LOG_TRACE_CONCAT("Set Viewport: ", x, ",", y, ",", width, ",", height);
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