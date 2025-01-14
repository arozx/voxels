#pragma once

#include <pch.h>
#include "../Events/Event.h"

namespace Engine {
    using EventCallbackFn = std::function<bool(Event&)>;

    /**
     * @brief Supported rendering APIs
     */
    enum class RenderAPI {
        None = 0,     ///< No rendering API
        OpenGL,       ///< OpenGL rendering
        Vulkan,       ///< Vulkan rendering
        DirectX11     ///< DirectX 11 rendering
    };

    /**
     * @brief Window creation properties
     */
    struct WindowProps {
        std::string Title;    ///< Window title
        uint32_t Width;      ///< Window width
        uint32_t Height;     ///< Window height
        RenderAPI API;       ///< Rendering API to use

        /**
         * @brief Constructs window properties
         * @param title Window title
         * @param width Window width
         * @param height Window height
         * @param api Rendering API
         */
        WindowProps(const std::string& title = "Voxel Engine",
                   uint32_t width = 1280,
                   uint32_t height = 720,
                   RenderAPI api = RenderAPI::OpenGL)
            : Title(title), Width(width), Height(height), API(api) {}
    };

    /**
     * @brief Abstract window interface
     * 
     * Provides platform-independent window functionality
     */
    class Window {
    public:
        virtual ~Window() = default;
        
        /** @brief Updates window state */
        virtual void OnUpdate() = 0;
        /** @return Window width */
        virtual uint32_t GetWidth() const = 0;
        /** @return Window height */
        virtual uint32_t GetHeight() const = 0;
        
        /** @brief Sets vertical sync mode */
        virtual void SetVSync(bool enabled) = 0;
        /** @return Whether VSync is enabled */
        virtual bool IsVSync() const = 0;

        /** @brief Sets clear color */
        virtual void SetClear(float a, float b, float c, float d) = 0;

        /** @brief Sets rendering context */
        virtual void SetContext() = 0;
        /** @brief Creates vertex buffer object */
        virtual void CreateVBO() = 0;
        
        /** @return Native window handle */
        virtual void* GetNativeWindow() const = 0;
        
        /** @brief Sets event callback function */
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        /**
         * @brief Creates platform-specific window
         * @param props Window properties
         * @return New window instance
         */
        static Window* Create(const WindowProps& props = WindowProps());
    };
}