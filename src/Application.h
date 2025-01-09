#pragma once

namespace Engine {
    class Application {
    public:
        Application();
        virtual ~Application();
        
        void Run();
    };

    // To be defined by client application
    Application* CreateApplication();
}