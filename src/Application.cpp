#include <pch.h>

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
#include "Scene/Scene.h"
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
     * @brief Constructs the Application instance and initializes core engine subsystems.
     *
     * This constructor sets up the runtime environment for the voxel engine by:
     * - Assigning the global instance pointer.
     * - Beginning a runtime profiling session.
     * - Creating the application window.
     * - Initializing the renderer and input system.
     * - Configuring the ImGui layer and overlay for UI rendering.
     * - Initializing the Lua script system and executing initial scripts from designated asset directories.
     * - Preloading frequently used assets and setting up default key toggle states.
     *
     * Critical initialization failures are logged to aid in troubleshooting startup issues.
     */
    Application::Application() 
    {
        s_Instance = this;
        LOG_TRACE("Creating Application");

        Engine::Profiler::Get().BeginSession("Runtime");

        InitWindow("Voxel Engine", 1280, 720);

        // Initialize renderer before other systems
        m_Renderer = std::make_unique<Renderer>();
        m_Renderer->Initialize();

        m_InputSystem = std::make_unique<InputSystem>(m_Window.get(), *m_Renderer);
        if (!m_InputSystem) {
            LOG_ERROR("Input system initialisation failed");
        }


        // Create and initialize ImGui layer first
        m_ImGuiLayer = std::make_unique<ImGuiLayer>(m_Window.get());
        
        if (!m_ImGuiLayer) {
            LOG_FATAL("Imgui layer init failed");
        }

        m_ImGuiLayer->Init(m_Window.get());  // Explicitly call Init
        LOG_TRACE("ImGui initialized");

        m_ImGuiOverlay = std::make_unique<ImGuiOverlay>(m_Window.get());

        if (!m_ImGuiOverlay) {
            LOG_FATAL("ImGui layer init failed");
        }

        // Initialize script system first
        m_ScriptSystem = std::make_unique<LuaScriptSystem>();
        if (!m_ScriptSystem) {
            LOG_ERROR("Failed to create script system");
            return;
        }
        
        try {
            m_ScriptSystem->Initialize();

            // Only execute main.lua once from the build directory
            LOG_INFO("Loading main.lua script...");
            if (!m_ScriptSystem->ExecuteFile("build/assets/scripts/main.lua")) {
                LOG_ERROR("Failed to execute main.lua");
                return;
            }

        } catch (const std::exception& e) {
            LOG_ERROR_CONCAT("Failed to initialize script system: ", e.what());
            return;
        }
        
        // Initialize remaining systems after script system is ready
        AssetManager::Get().PreloadFrequentAssets();

        InitializeToggleStates();

        m_TerrainSystem = nullptr;
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
     * @brief Executes the main application loop, managing rendering, input processing, and system updates.
     * 
     * @details This method is the core of the application's runtime behavior. It:
     * - Manages the application's main event loop
     * - Processes system updates at each frame
     * - Handles input and toggle states
     * - Manages rendering and scene management
     * - Supports performance profiling and debug features
     * 
     * @note The loop continues until `m_Running` is false or the window is closed
     * 
     * @pre Application systems must be initialized before calling
     * @pre Window and input systems must be operational
     * 
     * @performance O(1) per frame, with complexity dependent on scene and system updates
     * 
     * @thread_safety Not thread-safe; should be called from the main thread
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
     * @brief Initializes the application window with specified parameters and sets up event handling
     * 
     * Creates a window using the provided title, width, and height. Configures an event callback
     * to handle window events such as resizing and input events (key presses, mouse movements).
     * Registered events are cloned and pushed to the global event queue for further processing.
     * 
     * @param title The title of the window to be created
     * @param width The width of the window in pixels
     * @param height The height of the window in pixels
     * 
     * @note Logs window creation details using trace-level logging
     * @note Sets the window context after creation
     * 
     * @see Window
     * @see Event
     * @see EventQueue
     */
    void Application::InitWindow(const char* title, int width, int height) {
        LOG_TRACE_CONCAT("Creating window: ", title, ", Resolution: ", width, "x", height);
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

    /**
     * @brief Begins the rendering scene for the current frame.
     *
     * This method prepares the rendering pipeline by performing several key steps:
     * 1. Clears the renderer with a dark gray background color
     * 2. Renders the current scene through the SceneManager
     * 3. Initializes the ImGui layer for overlay rendering
     * 4. If ImGui is enabled, renders various debug and performance overlays
     *
     * @note Uses profiling to track performance of the scene beginning process
     * @note Conditionally renders ImGui overlays based on m_ImGuiEnabled flag
     *
     * Rendered overlays include:
     * - FPS counter
     * - Performance profiler
     * - Renderer settings
     * - Event debugger
     * - Terrain controls
     */
    void Application::BeginScene() {
        PROFILE_FUNCTION();

        m_Renderer->Clear({0.1f, 0.1f, 0.1f, 1.0f});
        m_ImGuiLayer->Begin();

        if (m_ImGuiEnabled) {
            // Always show these controls
            m_ImGuiOverlay->RenderProfiler();
            m_ImGuiOverlay->RenderEventDebugger();

            if (m_ShowFPSCounter) {
                static RenderObject dummyRenderObject;
                m_ImGuiOverlay->OnRender(
                    dummyRenderObject, m_ShowFPSCounter, m_FPSCounter.GetCurrentFPS(),
                    m_FPSCounter.GetAverageFPS(), m_FPSCounter.GetFrameTime(),
                    m_FPSCounter.GetOnePercentLow(), m_FPSCounter.GetOnePercentHigh());
            }

            // Only show these controls in 3D mode
            if (m_RenderType == RenderType::Render3D) {
                m_ImGuiOverlay->RenderRendererSettings();
                m_ImGuiOverlay->RenderTerrainControls();
            }
        }

        // Update the Scene
        if (m_ScriptSystem) {
            m_ScriptSystem->CallGlobalFunction("UpdateScene");
        }
    }

    /**
     * @brief Renders the current frame by invoking the renderer's drawing method.
     * 
     * This method triggers the drawing process for the current frame, delegating 
     * the rendering task to the renderer instance. It is typically called as part 
     * of the application's rendering pipeline to display the graphical output.
     * 
     * @note Uses the PROFILE_FUNCTION() macro for performance profiling.
     * 
     * @pre A valid renderer instance must be initialized and available.
     * @post The current frame is rendered and prepared for display.
     */
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

    void Application::ConfigureForRenderType() {
        auto activeScene = SceneManager::Get().GetActiveScene();
        if (!activeScene) return;

        switch (m_RenderType) {
            case RenderType::Render2D:
                if (m_TerrainSystem) {
                    m_TerrainSystem->Shutdown();
                    m_TerrainSystem = nullptr;
                }
                activeScene->SetCameraType(CameraType::Orthographic);
                break;

            case RenderType::Render3D:
                if (!m_TerrainSystem && activeScene) {
                    m_TerrainSystem = activeScene->GetTerrainSystem();
                    if (m_TerrainSystem) {
                        m_TerrainSystem->Initialize(GetRenderer());
                    }
                }
                activeScene->SetCameraType(CameraType::Perspective);
                break;
        }
    }

    void Application::ConfigureCamera() {
        if (!m_Renderer) {
            LOG_ERROR("Renderer not initialized in ConfigureCamera");
            return;
        }

        Renderer::CameraType rendererCamType = static_cast<Renderer::CameraType>(m_CameraType);
        LOG_TRACE_CONCAT("Camera type set to: ",
                         (rendererCamType == Renderer::CameraType::Orthographic ? "orthographic"
                                                                                : "perspective"));
        m_Renderer->SetCameraType(rendererCamType);

        // Create cameras if they don't exist
        if (m_CameraType == CameraType::Perspective) {
            if (!m_Renderer->GetPerspectiveCamera()) {
                m_Renderer->SetPerspectiveCamera(
                    std::make_shared<PerspectiveCamera>(45.0f, 1280.0f / 720.0f));
            }
            m_Renderer->GetPerspectiveCamera()->SetPosition({0.0f, 5.0f, -10.0f});
        } else {
            if (!m_Renderer->GetCamera()) {
                m_Renderer->SetCamera(
                    std::make_shared<OrthographicCamera>(-1.6f, 1.6f, -0.9f, 0.9f));
            }
            m_Renderer->GetCamera()->SetPosition({0.0f, 0.0f, 0.0f});
        }
    }
}