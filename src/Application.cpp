#include <GLFW/glfw3.h>
#include "Application.h"
#include "Logging.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"
#include "Events/KeyCodes.h"
#include <imgui.h>

namespace Engine {
    Application::Application() {
        LOG_INFO("Creating Application");
        InitWindow("Voxel Engine", 1280, 720);
        if (!m_Window) {
            LOG_ERROR("Failed to create window!");
            m_Running = false;
            return;
        }

        m_Window->SetContext();
        
        // Initialize GLAD
        /*
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            LOG_ERROR("Failed to initialize GLAD!");
            m_Running = false;
            return;
        }
        */

        // Now we can make OpenGL calls safely
        m_Window->SetClear(0.1f, 0.1f, 0.1f, 1.0f);

        m_ImGuiLayer = std::make_unique<ImGuiLayer>();
        m_ImGuiLayer->Init(m_Window.get());

        m_Renderer.Init();
    };

    Application::~Application() {
        if (m_ImGuiLayer) {
            m_ImGuiLayer->Shutdown();
        }
        LOG_INFO("Application Destroyed");
    }

    void Application::Run() {
        LOG_INFO("Application Starting...");
        
        bool show_demo_window = true;

        while (m_Running && m_Window) {
            m_Window->SetClear(0.1f, 0.1f, 0.1f, 1.0f);

            BeginScene();
            
            m_Renderer.Draw();

            // Start ImGui frame
            m_ImGuiLayer->Begin();

            // ImGui demo window
            if (show_demo_window) {
                ImGui::ShowDemoWindow(&show_demo_window);
            }

            // End ImGui frame
            m_ImGuiLayer->End();

            EndScene();

            // Update window (this calls glfwSwapBuffers and glfwPollEvents internally)
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
                    LOG_INFO("Window Close Event received");
                    m_Running = false;
                    return true;
                })
            );

            if (!e.IsHandled()) {
                switch (e.GetEventType()) {
                    case EventType::KeyPressed: {
                        const auto& ke = static_cast<const KeyPressedEvent&>(e);
                        std::string keyName = KeyCodeToString(ke.GetKeyCode());
                        // LOG_INFO_CONCAT("Key pressed: ", keyName, " (keycode: ", ke.GetKeyCode(), ")",
                        //     ke.IsRepeat() ? " (repeat)" : "");
                        break;
                    }
                    case EventType::KeyReleased: {
                        const auto& ke = static_cast<const KeyReleasedEvent&>(e);
                        std::string keyName = KeyCodeToString(ke.GetKeyCode());
                        // LOG_INFO_CONCAT("Key released: ", keyName, " (keycode: ", ke.GetKeyCode(), ")");
                        break;
                    }
                    case EventType::MouseMoved: {
                        const auto& me = static_cast<const MouseMovedEvent&>(e);
                        // LOG_INFO_CONCAT("Mouse moved to: (", me.GetX(), ", ", me.GetY(), ")");
                        break;
                    }
                    case EventType::MouseButtonPressed: {
                        const auto& me = static_cast<const MouseButtonPressedEvent&>(e);
                        LOG_INFO_CONCAT("Mouse button pressed: ", me.GetButton());
                        break;
                    }
                    case EventType::MouseButtonReleased: {
                        const auto& me = static_cast<const MouseButtonReleasedEvent&>(e);
                        // LOG_INFO_CONCAT("Mouse button released: ", me.GetButton());
                        break;
                    }
                    case EventType::MouseScrolled: {
                        const auto& me = static_cast<const MouseScrolledEvent&>(e);
                        // LOG_INFO_CONCAT("Mouse scrolled: X:", me.GetXOffset(), " Y:", me.GetYOffset());
                        break;
                    }
                    default:
                        LOG_INFO_CONCAT("Event: ", e.GetName());
                        break;
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
        
        // TODO: draw a triange here

    }

    void Application::EndScene() {
        // LOG_TRACE("End Scene");
    }

    void Application::Present() {
        // LOG_TRACE("Present Frame");
    }

    void Application::SetViewport(int x, int y, int width, int height) {
        LOG_TRACE_CONCAT("Set Viewport: ", x, ",", y, ",", width, ",", height);
    }
}  // namespace Engine