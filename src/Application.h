#pragma once

#include "Window/Window.h"
#include "ImGui/ImGuiLayer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Material.h"
#include "VoxelTerrain.h"
#include "TerrainSystem/TerrainSystem.h"
#include "Input/InputSystem.h"
#include "Scene/SceneManager.h"

namespace Engine {
    class Application {
    public:
        Application();
        virtual ~Application();
        
        void Run();
        
        unsigned int indicies;
    protected:
        void InitWindow(const char* title = "Voxel Engine", int width = 1280, int height = 720);
        void ShutdownWindow();
        
        void BeginScene();
        void EndScene();
        void Present();
        void SetViewport(int x, int y, int width, int height);
        
        // Add new function to create a test square using file shaders
        void CreateFileShaderSquare();

        // Add missing function declarations
        void CreatePixelatedSquare();
        void CreateWaveDissolveSquare();
        void CreateBlurSquare();

    private:
        bool m_Running = true;
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
        std::unique_ptr<InputSystem> m_InputSystem;
        std::unique_ptr<TerrainSystem> m_TerrainSystem;
        Renderer m_Renderer;

        // Add all the missing member variables
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Material> m_Material;

        std::shared_ptr<VertexArray> m_SquareVA;
        std::shared_ptr<Shader> m_SquareShader;
        std::shared_ptr<Material> m_SquareMaterial;
        std::shared_ptr<Texture> m_TestTexture;
        Transform m_SquareTransform;

        std::shared_ptr<VertexArray> m_TransparentSquareVA;
        std::shared_ptr<Shader> m_TransparentShader;
        std::shared_ptr<Material> m_TransparentMaterial;
        Transform m_TransparentTransform;

        std::shared_ptr<VertexArray> m_FileShaderSquareVA;
        std::shared_ptr<Shader> m_FileShaderSquareShader;
        std::shared_ptr<Material> m_FileShaderSquareMaterial;
        Transform m_FileShaderSquareTransform;

        std::shared_ptr<VertexArray> m_PixelatedSquareVA;
        std::shared_ptr<Shader> m_PixelatedShader;
        std::shared_ptr<Material> m_PixelatedMaterial;
        Transform m_PixelatedTransform;

        std::shared_ptr<VertexArray> m_WaveDissolveVA;
        std::shared_ptr<Shader> m_WaveDissolveShader;
        std::shared_ptr<Material> m_WaveDissolveMaterial;
        Transform m_WaveDissolveTransform;

        std::shared_ptr<VertexArray> m_BlurVA;
        std::shared_ptr<Shader> m_BlurShader;
        std::shared_ptr<Material> m_BlurMaterial;
        Transform m_BlurTransform;

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
    };
    
    // To be defined by client application
    Application* CreateApplication();
}