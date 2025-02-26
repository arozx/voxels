#include "OpenGLWindow.h"

#include <pch.h>

#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"
#include "../Events/WindowEvent.h"

namespace Engine {

    /**
     * @brief Create a new Window instance
     * @param props Window properties for initialization
     * @return Pointer to created Window instance
     */
    Window* Window::Create(const WindowProps& props) {
        return new OpenGLWindow(props);
    }

    /**
     * @brief OpenGL-specific window implementation
     * @details Handles window creation, event callbacks, and OpenGL context management
     */
    OpenGLWindow::OpenGLWindow(const WindowProps& props) {
        Init(props);
    }

    /**
     * @brief Clean up window resources
     * @details Destroys GLFW window and terminates GLFW
     */
    OpenGLWindow::~OpenGLWindow() {
        Shutdown();
    }

    /**
     * @brief Initialize the OpenGL window with specified properties
     * 
     * @param props Window configuration parameters including title, width, and height
     * 
     * @details Performs comprehensive window initialization:
     * - Initializes GLFW library
     * - Configures OpenGL context version and profile
     * - Creates GLFW window
     * - Sets up window context
     * - Registers multiple input callbacks for window events, keyboard, mouse, and scroll interactions
     * 
     * @note Terminates GLFW and logs an error if window creation fails
     * @note Enables vertical synchronization by default after initialization
     * 
     * @throws None Handles initialization errors internally
     */
    void OpenGLWindow::Init(const WindowProps& props) {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        if (!glfwInit()) {
            LOG_ERROR("Could not initialize GLFW!");
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), nullptr, nullptr);
        
        if (!m_Window) {
            LOG_ERROR("Failed to create GLFW window!");
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_Window);

        // Store window data for callbacks
        glfwSetWindowUserPointer(m_Window, &m_Data);

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });

        // Keyboard callback with more detail
        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            
            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, false);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, true);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        // Mouse button callbacks with position
        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            
            switch (action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xpos, (float)ypos);
            data.EventCallback(event);
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)xoffset, (float)yoffset);
            data.EventCallback(event);
        });

        SetVSync(true);
    }

    /**
     * @brief Clean up window resources
     * @details Destroys GLFW window and terminates GLFW
     */
    void OpenGLWindow::Shutdown() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
        glfwTerminate();
    }

    /**
     * @brief Update window events and swap buffers
     */
    void OpenGLWindow::OnUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    /**
     * @brief Enable or disable vertical synchronization
     * @param enabled True to enable VSync, false to disable
     */
    void OpenGLWindow::SetVSync(bool enabled) {
        glfwSwapInterval(enabled ? 1 : 0);
        m_Data.VSync = enabled;
    }

    /**
     * @brief Check if VSync is enabled
     * @return True if VSync is enabled, false otherwise
     */
    bool OpenGLWindow::IsVSync() const {
        return m_Data.VSync;
    }

    /**
     * @brief Set clear color and clear buffers
     * @param a Red component (0-1)
     * @param b Green component (0-1)
     * @param c Blue component (0-1)
     * @param d Alpha component (0-1)
     */
    void OpenGLWindow::SetClear(float a, float b, float c, float d) {
        glClearColor(a, b, c, d);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    /**
     * @brief Set up OpenGL context and enable necessary features
     * @details Enables depth testing, blending, and face culling
     */
    void OpenGLWindow::SetContext() {
        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        
        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Enable face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        glfwMakeContextCurrent(m_Window);
    }

    /**
     * @brief Create Vertex Buffer Object
     * @note Currently empty implementation
     */
    void OpenGLWindow::CreateVBO() {
    }
}