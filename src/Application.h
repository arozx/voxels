#pragma once
#include "Window/Window.h"
#include <memory>

namespace Engine {
    class Application {
    public:
        Application();
        virtual ~Application();
        
        void Run();
        
    protected:
        void InitWindow(const char* title = "Voxel Engine", int width = 1280, int height = 720);
        void ShutdownWindow();
        
        void BeginScene();
        void EndScene();
        void Present();
        void SetClearColor(float r, float g, float b, float a = 1.0f);
        void Clear();
        void SetViewport(int x, int y, int width, int height);
        
    private:
        bool m_Running = true;
        std::unique_ptr<Window> m_Window;
    };

    // To be defined by client application
    Application* CreateApplication();
}