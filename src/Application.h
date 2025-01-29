#pragma once

#include <sol.hpp>

#include "Camera/CameraTypes.h"  // Add this include
#include "Core/AssetManager.h"
#include "Core/FPSCounter.h"
#include "ImGui/ImGuiLayer.h"
#include "Input/InputSystem.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Renderer/RenderableObject.h"
#include "Renderer/Renderer.h"
#include "Scene/SceneManager.h"
#include "Scripting/LuaScriptSystem.h"
#include "UI/ImGuiOverlay.h"
#include "Window/Window.h"

namespace Engine {
enum class CameraType;  // Forward declare

enum class RenderType { Render2D, Render3D };

/**
 * @brief Stores state for key toggle functionality
 */
struct KeyToggleState {
    bool previousState = false;   ///< Previous key state
    float pressStartTime = 0.0f;  ///< Time when key was pressed
    bool currentValue = true;     ///< Current toggle state
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

        Renderer& GetRenderer() { return *m_Renderer; }

        LuaScriptSystem* GetScriptSystem() { return m_ScriptSystem.get(); }

        static Application& Get() { return *s_Instance; }
        InputSystem* GetInputSystem() { return m_InputSystem.get(); }

        virtual void OnImGuiRender() {}

        const std::string& GetAssetPath() const { return m_AssetPath; }

        ImGuiOverlay* GetImGuiOverlay() { return m_ImGuiOverlay.get(); }

        void SetRenderType(RenderType type) {
            m_RenderType = type;
            ConfigureForRenderType();
        }
        RenderType GetRenderType() const { return m_RenderType; }

        void SetCameraType(CameraType type) {
            m_CameraType = type;
            ConfigureCamera();
        }
        CameraType GetCameraType() const { return m_CameraType; }

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
        void SetViewport(int x, int y, int width, int height) {
            m_Renderer->SetViewport(x, y, width, height);
        }

        /**
         * @brief Process pending events
         */
        void ProcessEvents();

        void ConfigureForRenderType();

        std::unique_ptr<Renderer> m_Renderer;
        Engine::TerrainSystem* m_TerrainSystem = nullptr;

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

        // Rendering objects
        std::vector<std::unique_ptr<RenderableObject>> m_RenderableObjects;

        // FPS tracking members
        bool m_ShowFPSCounter = true;
        FPSCounter m_FPSCounter;

        std::unique_ptr<ImGuiOverlay> m_ImGuiOverlay;

        void UpdateFPSCounter(float deltaTime);

        std::unique_ptr<Light> m_Light;

        // scripting
        std::unique_ptr<LuaScriptSystem> m_ScriptSystem;

        static Application* s_Instance;

        std::string m_AssetPath = "../sandbox/assets/";  // Base path for all assets

        RenderType m_RenderType = RenderType::Render3D;

        CameraType m_CameraType = CameraType::Perspective;

        void ConfigureCamera();
    };
    
    // To be defined by client application
    Application* CreateApplication();
}