#include "pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Application.h"
#include "Logging.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"
#include "Events/KeyCodes.h"
#include "Camera/OrthographicCamera.h"
#include "Renderer/VertexArray.h"
#include <imgui.h>
#include "Shader/DefaultShaders.h"
#include "Renderer/MeshTemplates.h"

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

        m_ImGuiLayer = std::make_unique<ImGuiLayer>();
        m_ImGuiLayer->Init(m_Window.get());

        m_Renderer.Init();

        // Create shader for triangle
        m_Shader = std::make_shared<Shader>(
            DefaultShaders::BasicVertexShader, 
            DefaultShaders::SimpleColorFragmentShader);
        m_Material = std::make_shared<Material>(m_Shader);
        m_Material->SetVector4("u_Color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));

        // Create triangle mesh using template
        auto triangleVertices = MeshTemplates::GetVertexData(MeshTemplates::Triangle);
        m_VertexArray.reset(VertexArray::Create());
        
        std::shared_ptr<VertexBuffer> vertexBuffer(
            VertexBuffer::Create(triangleVertices.data(), 
            triangleVertices.size() * sizeof(float)));
        
        BufferLayout layout = {
            { ShaderDataType::Float3, "aPosition" }
        };
        
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        std::shared_ptr<IndexBuffer> indexBuffer(
            IndexBuffer::Create(MeshTemplates::TriangleIndices.data(), 
            MeshTemplates::TriangleIndices.size()));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        // Create textured square
        auto squareVertices = MeshTemplates::TexturedSquare;
        m_SquareVA.reset(VertexArray::Create());
        
        std::shared_ptr<VertexBuffer> squareVB(
            VertexBuffer::Create(squareVertices.data(), 
            squareVertices.size() * sizeof(float)));

        BufferLayout squareLayout = {
            { ShaderDataType::Float3, "aPosition" },
            { ShaderDataType::Float2, "aTexCoord" }
        };
        
        squareVB->SetLayout(squareLayout);
        m_SquareVA->AddVertexBuffer(squareVB);

        std::shared_ptr<IndexBuffer> squareIB(
            IndexBuffer::Create(MeshTemplates::SquareIndices.data(), 
            MeshTemplates::SquareIndices.size()));
        m_SquareVA->SetIndexBuffer(squareIB);

        // Load texture
        m_TestTexture = Texture::Create("assets/textures/test.png");

        // Create shader for textured square
        m_SquareShader = std::make_shared<Shader>(
            DefaultShaders::TexturedVertexShader, 
            DefaultShaders::TexturedFragmentShader);
        m_SquareMaterial = std::make_shared<Material>(m_SquareShader);
        m_SquareMaterial->SetTexture("u_Texture", m_TestTexture);
        m_SquareMaterial->SetVector4("u_Color", glm::vec4(1.0f));

        // Create transform for square
        m_SquareTransform.position = glm::vec3(0.5f, 0.5f, 0.5f);
        // m_SquareTransform.rotation = glm::vec3(0.0f); // In radians
        m_SquareTransform.scale = glm::vec3(0.5f);
    }

    Application::~Application() {
        if (m_ImGuiLayer) {
            m_ImGuiLayer->Shutdown();
        }
        LOG_INFO("Application Destroyed");
    }

    void Application::Run() {
        LOG_INFO("Application Starting...");
        
        bool ImGuiEnabled = true;

        float lastFrameTime = 0.0f;

        while (m_Running && m_Window) {
            float time = (float)glfwGetTime();
            float deltaTime = time - lastFrameTime;
            lastFrameTime = time;

            // Calculate FPS
            m_FrameTime = deltaTime;
            m_FPSUpdateTimer += deltaTime;
            if (m_FPSUpdateTimer >= 0.5f) { // Update FPS twice per second
                m_FPS = 1.0f / m_FrameTime;
                m_FPSUpdateTimer = 0.0f;
            }

            // Toggle mouse control with right mouse button
            if (glfwGetMouseButton(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
                if (!m_MouseControlEnabled) {
                    m_MouseControlEnabled = true;
                    m_FirstMouse = true;
                    glfwSetInputMode(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
            } else {
                m_MouseControlEnabled = false;
                glfwSetInputMode(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            // Camera type switching
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_1) == GLFW_PRESS) {
                m_Renderer.SetCameraType(Renderer::CameraType::Orthographic);
                LOG_INFO("Switched to Orthographic Camera");
            }
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_2) == GLFW_PRESS) {
                m_Renderer.SetCameraType(Renderer::CameraType::Perspective);
                LOG_INFO("Switched to Perspective Camera");
            }

            // Camera Movement
            if (m_Renderer.GetCameraType() == Renderer::CameraType::Orthographic) {
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_W) == GLFW_PRESS)
                    m_Renderer.GetCamera()->MoveUp(deltaTime);
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_S) == GLFW_PRESS)
                    m_Renderer.GetCamera()->MoveDown(deltaTime);
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_A) == GLFW_PRESS)
                    m_Renderer.GetCamera()->MoveLeft(deltaTime);
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_D) == GLFW_PRESS)
                    m_Renderer.GetCamera()->MoveRight(deltaTime);
            } else {
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_W) == GLFW_PRESS)
                    m_Renderer.GetPerspectiveCamera()->MoveForward(deltaTime);
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_S) == GLFW_PRESS)
                    m_Renderer.GetPerspectiveCamera()->MoveBackward(deltaTime);
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_A) == GLFW_PRESS)
                    m_Renderer.GetPerspectiveCamera()->MoveLeft(deltaTime);
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_D) == GLFW_PRESS)
                    m_Renderer.GetPerspectiveCamera()->MoveRight(deltaTime);
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_SPACE) == GLFW_PRESS)
                    m_Renderer.GetPerspectiveCamera()->MoveUp(deltaTime);
                if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                    m_Renderer.GetPerspectiveCamera()->MoveDown(deltaTime);
            }

            // Update transform (TRS)
            // m_SquareTransform.rotation.z = time; // Rotate around Z axis
            // float scale = (sin(time) + 2.0f) * 0.5f; // Scale between 0.5 and 1.5
            // m_SquareTransform.scale = glm::vec3(scale);
            m_SquareTransform.scale = glm::vec3(1.0f); // Or whatever fixed scale you want

            // Add WASD movement for the square
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_A) == GLFW_PRESS)
                m_SquareTransform.position.x -= 2.0f * deltaTime;
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_D) == GLFW_PRESS)
                m_SquareTransform.position.x += 2.0f * deltaTime;
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_W) == GLFW_PRESS)
                m_SquareTransform.position.y += 2.0f * deltaTime;
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_S) == GLFW_PRESS)
                m_SquareTransform.position.y -= 2.0f * deltaTime;

            // Add manual rotation controls with Q and E keys
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_Q) == GLFW_PRESS)
                m_SquareTransform.rotation.z += 2.0f * deltaTime;
            if (glfwGetKey(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), GLFW_KEY_E) == GLFW_PRESS)
                m_SquareTransform.rotation.z -= 2.0f * deltaTime;

            BeginScene();
            
            if (ImGuiEnabled) {
                ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("Camera Type: %s", m_Renderer.GetCameraType() == Renderer::CameraType::Orthographic ? "Orthographic" : "Perspective");
                ImGui::Text("FPS: %.1f (%.2f ms)", m_FPS, m_FrameTime * 1000.0f);
                ImGui::End();

                ImGui::Begin("Transform Controls");
                ImGui::DragFloat3("Position", &m_SquareTransform.position[0], 0.1f);
                ImGui::DragFloat3("Rotation", &m_SquareTransform.rotation[0], 0.1f);
                ImGui::DragFloat3("Scale", &m_SquareTransform.scale[0], 0.1f);
                ImGui::End();
            }

            EndScene();

            Present();
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
                        if (m_MouseControlEnabled) {
                            const auto& me = static_cast<const MouseMovedEvent&>(e);
                            if (m_FirstMouse) {
                                m_LastMouseX = me.GetX();
                                m_LastMouseY = me.GetY();
                                m_FirstMouse = false;
                            }

                            float xOffset = me.GetX() - m_LastMouseX;
                            float yOffset = m_LastMouseY - me.GetY();

                            m_LastMouseX = me.GetX();
                            m_LastMouseY = me.GetY();

                            if (m_Renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                                m_Renderer.GetPerspectiveCamera()->RotateWithMouse(xOffset, yOffset, 0.1f);
                            }
                        }
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
        //* OPTIONAL: make magenta to make it clear when clear is rendered
        // RGB Alpha
        // m_Window->SetClear(1.0f, 0.0f, 1.0f, 0.0f);
        m_Window->SetClear(0.1f, 0.1f, 0.1f, 1.0f); // Dark Grey
        m_Renderer.Submit(m_VertexArray, m_Material);
        m_Renderer.Submit(m_SquareVA, m_SquareMaterial, m_SquareTransform);
        m_Renderer.Draw();

        m_ImGuiLayer->Begin();
    }

    void Application::EndScene() {
        m_ImGuiLayer->End();
    }

    void Application::Present() {
        m_Window->OnUpdate();
    }

    void Application::SetViewport(int x, int y, int width, int height) {
        LOG_TRACE_CONCAT("Set Viewport: ", x, ",", y, ",", width, ",", height);
    }
}