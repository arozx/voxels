#pragma once

#include "Window/Window.h"
#include "ImGui/ImGuiLayer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Material.h"
#include "VoxelTerrain.h"
#include "TerrainSystem/TerrainSystem.h"
#include "Input/InputSystem.h"
#include "Scene/SceneManager.h"
#include "UI/ImGuiOverlay.h"
#include "Renderer/RenderableObject.h"
#include "Core/AssetManager.h"

namespace Engine {
    /**
     * @brief Stores state for key toggle functionality
     */
    struct KeyToggleState {
        bool previousState = false;     ///< Previous key state
        float pressStartTime = 0.0f;    ///< Time when key was pressed
        bool currentValue = false;      ///< Current toggle state
    };

    /**
     * @brief Main application class handling window, rendering and game loop
     */
    class Application {
    public:
        Application();
        virtual ~Application();
        
        /**
         * @brief Main application loop
         */
        void Run();
        
        unsigned int indicies;
    protected:
        /**
         * @brief Initialize the application window
         * @param title Window title
         * @param width Window width
         * @param height Window height
         */
        void InitWindow(const char* title = "Voxel Engine", int width = 1280, int height = 720);

        /**
         * @brief Clean up window resources
         */
        void ShutdownWindow();
        
        /**
         * @brief Begin a new frame
         */
        void BeginScene();

        /**
         * @brief End the current frame
         */
        void EndScene();

        /**
         * @brief Present the rendered frame
         */
        void Present();

        /**
         * @brief Set the viewport dimensions
         */
        void SetViewport(int x, int y, int width, int height);
        
        /**
         * @brief Process pending events
         */
        void ProcessEvents();

        // Add new function to create a test square using file shaders
        void CreateFileShaderSquare();

        void CreatePixelatedSquare();
        void CreateWaveDissolveSquare();
        void CreateBlurSquare();

    private:
        /**
         * @brief Handle key toggle state changes
         * @param key GLFW key code
         * @param currentTime Current time
         * @return bool True if toggle state changed
         */
        bool HandleKeyToggle(int key, float currentTime);

        /**
         * @brief Initialize toggle states for keys
         */
        void InitializeToggleStates();

        /**
         * @brief Add a new key toggle state
         * @param key GLFW key code
         * @param defaultValue Initial toggle state
         */
        void AddToggleState(int key, bool defaultValue = false);

        /**
         * @brief Remove a key toggle state
         * @param key GLFW key code
         */
        void RemoveToggleState(int key);

        /**
         * @brief Get current toggle state for a key
         * @param key GLFW key code
         * @return bool Current toggle state
         */
        bool GetToggleState(int key) const;

        /**
         * @brief Set toggle state for a key
         * @param key GLFW key code
         * @param value New toggle state
         */
        void SetToggleState(int key, bool value);

        bool m_Running = true;
        bool m_ImGuiEnabled = true;
        
        float m_DebounceTime = 0.3f;

        std::unordered_map<int, KeyToggleState> m_KeyToggles;

        const float MAX_TOGGLE_HOLD_TIME = 1.5f;

        std::unique_ptr<Window> m_Window;
        std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
        std::unique_ptr<InputSystem> m_InputSystem;
        std::unique_ptr<TerrainSystem> m_TerrainSystem;
        Renderer m_Renderer;

        // Rendering objects
        std::vector<std::unique_ptr<RenderableObject>> m_RenderableObjects;
        std::unique_ptr<RenderableObject> m_Triangle;
        std::unique_ptr<RenderableObject> m_TexturedSquare;  // This will be our active square for movement
        std::unique_ptr<RenderableObject> m_TransparentSquare;
        std::unique_ptr<RenderableObject> m_FileShaderSquare;
        std::unique_ptr<RenderableObject> m_PixelatedSquare;
        std::unique_ptr<RenderableObject> m_WaveDissolveSquare;
        std::unique_ptr<RenderableObject> m_BlurSquare;

        std::shared_ptr<Texture> m_TestTexture;
        std::shared_ptr<Texture> m_LargeTexture;

        // FPS tracking members
        bool m_ShowFPSCounter = true;
        static const size_t FPS_SAMPLE_COUNT = 200; // Store last ~3.3 seconds at 60fps
        std::vector<float> m_FPSSamples;
        float m_FPS = 0.0f;
        float m_CurrentFPS = 0.0f;
        float m_FPS1PercentHigh = 0.0f;
        float m_FPS1PercentLow = 0.0f;
        float m_FrameTime = 0.0f;
        float m_FPSUpdateTimer = 0.0f;
        size_t m_CurrentFPSSample = 0;

        std::unique_ptr<ImGuiOverlay> m_ImGuiOverlay;

        void UpdateFPSCounter(float deltaTime, float currentTime);
    };
    
    // To be defined by client application
    Application* CreateApplication();
}