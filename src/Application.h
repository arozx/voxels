#pragma once
#include "Window/Window.h"
#include "ImGui/ImGuiLayer.h"
#include "Renderer/Renderer.h"

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
        
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<Shader> m_Shader;
        Renderer m_Renderer;

        float m_LastMouseX = 0.0f;
        float m_LastMouseY = 0.0f;
        bool m_FirstMouse = true;
        bool m_MouseControlEnabled = false;
    };

    // To be defined by client application
    Application* CreateApplication();
}