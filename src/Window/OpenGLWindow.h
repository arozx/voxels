#pragma once
#include "Window.h"
#include <GLFW/glfw3.h>
#include <cstdint>

namespace Engine {
    class OpenGLWindow : public Window {
    public:
        OpenGLWindow(const WindowProps& props);
        virtual ~OpenGLWindow();

        void OnUpdate() override;
        
        inline uint32_t GetWidth() const override { return m_Data.Width; }
        inline uint32_t GetHeight() const override { return m_Data.Height; }
        
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;
        
        virtual void* GetNativeWindow() const override { return m_Window; }
        void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
        
    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();
        
        GLFWwindow* m_Window;
        
        struct WindowData {
            std::string Title;
            uint32_t Width, Height;
            bool VSync;
            EventCallbackFn EventCallback;
            
            WindowData()
                : Title(""), Width(0), Height(0), VSync(false) {}
        };
        
        WindowData m_Data;
    };
}