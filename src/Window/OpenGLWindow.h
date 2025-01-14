#pragma once
#include <pch.h>
#include "Window.h"
#include <GLFW/glfw3.h>

namespace Engine {
    /**
     * @brief OpenGL-specific window implementation
     * 
     * Provides GLFW-based window implementation with OpenGL context
     */
    class OpenGLWindow : public Window {
    public:
        /**
         * @brief Constructs OpenGL window
         * @param props Window properties
         */
        OpenGLWindow(const WindowProps& props);
        virtual ~OpenGLWindow();

        void OnUpdate() override;
        
        inline uint32_t GetWidth() const override { return m_Data.Width; }
        inline uint32_t GetHeight() const override { return m_Data.Height; }
        
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        void SetClear(float a, float b, float c, float d);

        void SetContext();
        void CreateVBO(); // Vertex Buffer
        
        virtual void* GetNativeWindow() const override { return m_Window; }
        void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
        
    private:
        /**
         * @brief Initializes GLFW window and OpenGL context
         * @param props Window properties
         */
        virtual void Init(const WindowProps& props);
        
        /** @brief Cleans up window resources */
        virtual void Shutdown();
        
        GLFWwindow* m_Window;     ///< GLFW window handle
        
        /** @brief Internal window data structure */
        struct WindowData {
            std::string Title;     ///< Window title
            uint32_t Width;       ///< Window width
            uint32_t Height;      ///< Window height
            bool VSync;           ///< VSync enabled flag
            EventCallbackFn EventCallback;  ///< Event callback function
            
            WindowData()
                : Title(""), Width(0), Height(0), VSync(false) {}
        };
        
        WindowData m_Data;  ///< Window data
    };
}