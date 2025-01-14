#pragma once

#include <pch.h>
#include "../Events/Event.h"

namespace Engine {
    using EventCallbackFn = std::function<bool(Event&)>;

    enum class RenderAPI {
        None = 0,
        OpenGL,
        Vulkan,
        DirectX11
    };

    struct WindowProps {
        std::string Title;
        uint32_t Width;
        uint32_t Height;
        RenderAPI API;

        WindowProps(const std::string& title = "Voxel Engine",
                   uint32_t width = 1280,
                   uint32_t height = 720,
                   RenderAPI api = RenderAPI::OpenGL)
            : Title(title), Width(width), Height(height), API(api) {}
    };

    class Window {
    public:
        virtual ~Window() = default;
        
        virtual void OnUpdate() = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void SetClear(float a, float b, float c, float d) = 0;

        virtual void SetContext() = 0;
        virtual void CreateVBO() = 0;
        
        virtual void* GetNativeWindow() const = 0;
        
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        static Window* Create(const WindowProps& props = WindowProps());
    };
}