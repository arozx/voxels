#include "Application.h"
#include "Logging.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

namespace Engine {
    Application::Application() {
        LOG_INFO("Creating Application");
        InitWindow("Voxel Engine", 1280, 720);
        if (!m_Window) {
            LOG_ERROR("Failed to create window!");
            m_Running = false;
        }
    }

    Application::~Application() {
        LOG_INFO("Application Destroyed");
    }

    void Application::Run() {
        LOG_INFO("Application Starting...");
        
        while (m_Running && m_Window) {
            BeginScene();
            Clear();
            // Rendering will happen here
            EndScene();
            m_Window->OnUpdate();
        }
    }

    void Application::InitWindow(const char* title, int width, int height) {
        LOG_INFO("Initializing window: {0} ({1}x{2})", title, width, height);
        WindowProps props(title, width, height);
        m_Window = std::unique_ptr<Window>(Window::Create(props));
        
        // Set event callback
        m_Window->SetEventCallback([this](Event& e) {
            EventDispatcher dispatcher(e);
            
            dispatcher.Dispatch<WindowCloseEvent>(
                std::function<bool(WindowCloseEvent&)>([this](WindowCloseEvent& e) {
                    m_Running = false;
                    return true;
                })
            );

            // Handle other events if not already handled
            if (!e.IsHandled()) {
                if (e.GetEventType() == EventType::KeyPressed) {
                    const auto& ke = static_cast<const KeyPressedEvent&>(e);
                    LOG_INFO("Key pressed: {}", ke.GetKeyCode());
                }
                else if (e.GetEventType() == EventType::KeyReleased) {
                    const auto& ke = static_cast<const KeyReleasedEvent&>(e);
                    LOG_INFO("Key released: {}", ke.GetKeyCode());
                }
                else if (e.GetEventType() == EventType::MouseMoved) {
                    const auto& me = static_cast<const MouseMovedEvent&>(e);
                    LOG_INFO("Mouse moved: ({}, {})", me.GetX(), me.GetY());
                }
                else {
                    LOG_INFO("Event: {}", e.GetName());
                }
            }
        });
    }

    void Application::ShutdownWindow() {
        LOG_INFO("Shutting down window");
        m_Window.reset();
    }

    void Application::BeginScene() {
        // LOG_TRACE("Begin Scene");
    }

    void Application::EndScene() {
        // LOG_TRACE("End Scene");
    }

    void Application::Present() {
        // LOG_TRACE("Present Frame");
    }

    void Application::SetClearColor(float r, float g, float b, float a) {
        // LOG_TRACE("Set Clear Color: " + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "," + std::to_string(a));
    }

    void Application::Clear() {
        // LOG_TRACE("Clear");
    }

    void Application::SetViewport(int x, int y, int width, int height) {
        LOG_TRACE_CONCAT("Set Viewport: ", x, ",", y, ",", width, ",", height);
    }
}