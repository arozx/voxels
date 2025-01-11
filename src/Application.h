#pragma once
#include "Window/Window.h"
#include "ImGui/ImGuiLayer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Material.h"

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
        
    private:
        bool m_Running = true;
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
        
        // Add FPS counter variables
        float m_FPS = 0.0f;
        float m_FrameTime = 0.0f;
        float m_FPSUpdateTimer = 0.0f;

        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Material> m_Material;
        Renderer m_Renderer;

        std::shared_ptr<VertexArray> m_SquareVA;
        std::shared_ptr<Shader> m_SquareShader;
        std::shared_ptr<Material> m_SquareMaterial;
        Transform m_SquareTransform;

        std::shared_ptr<Texture> m_TestTexture;

        float m_LastMouseX = 0.0f;
        float m_LastMouseY = 0.0f;
        bool m_FirstMouse = true;
        bool m_MouseControlEnabled = false;
    };

    // To be defined by client application
    Application* CreateApplication();
}