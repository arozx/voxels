#pragma once
#include <GLFW/glfw3.h>
#include <pch.h>

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/Renderer.h"
#include "Window/Window.h"

namespace Engine {
/**
 * @brief Handles all input processing for the application
 *
 * Manages keyboard and mouse input, camera controls, and input event handling.
 * Provides an interface between GLFW input and the game systems.
 */
class InputSystem {
   public:
    /**
     * @brief Constructs an input system
     * @param window Pointer to the application window
     * @param renderer Reference to the renderer
     */
    InputSystem(Window* window, Renderer& renderer);

    /**
     * @brief Updates input state and handles continuous input
     * @param deltaTime Time elapsed since last update
     */
    void Update(float deltaTime);

    /**
     * @brief Processes input events
     * @param e Event to process
     */
    void OnEvent(Event& e);

    /**
     * @brief Checks if a key is currently pressed
     * @param keycode GLFW key code to check
     * @return true if key is pressed
     */

    bool IsKeyPressed(int keycode) const;

    /**
     * @brief Checks if a mouse button is currently pressed
     * @param button GLFW mouse button code to check
     * @return true if button is pressed
     */
    bool IsMouseButtonPressed(int button) const;

    /**
     * @brief Gets the current mouse cursor position
     * @return Pair of x,y coordinates
     */
    std::pair<float, float> GetMousePosition() const;

    /**
     * @brief Sets mouse sensitivity
     * @param sensitivity New sensitivity value
     */
    void SetSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }

    /** @return Current mouse sensitivity */
    float GetSensitivity() const { return m_MouseSensitivity; }

    // Add new control methods
    void ToggleCameraControls() { m_CameraEnabled = !m_CameraEnabled; }
    void ToggleSmoothCamera() { m_SmoothCamera = !m_SmoothCamera; }
    void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
    float GetMovementSpeed() const { return m_MovementSpeed; }
    void ToggleMovementLock() { m_MovementLocked = !m_MovementLocked; }

    // Key callback system
    using KeyCallback = std::function<void(int)>;
    void RegisterKeyCallback(int key, KeyCallback callback);

    // Key event handling
    void OnKey(int key, int scancode, int action, int mods);

   private:
    /**
     * @brief Handles mouse movement input
     * @param e Mouse moved event
     */
    void HandleMouseMovement(const MouseMovedEvent& e);

    /**
     * @brief Processes keyboard input
     * @param deltaTime Time elapsed since last update
     */
    void HandleKeyInput(float deltaTime);

    /**
     * @brief Updates camera position based on input
     * @param deltaTime Time elapsed since last update
     */
    void HandleCameraMovement(float deltaTime);

    Window* m_Window;      ///< Pointer to application window
    Renderer& m_Renderer;  ///< Reference to renderer

    bool m_FirstMouse = true;         ///< First mouse input flag
    float m_LastMouseX = 0.0f;        ///< Last mouse X position
    float m_LastMouseY = 0.0f;        ///< Last mouse Y position
    float m_MouseSensitivity = 0.1f;  ///< Mouse movement sensitivity

    // Camera control flags
    bool m_CameraEnabled = true;  ///< Toggle for camera controls
    bool m_SmoothCamera = true;   ///< Toggle for smooth camera movement

    // Movement timing
    const float FIXED_TIMESTEP = 1.0f / 60.0f;  ///< 60Hz update rate
    float m_MovementAccumulator = 0.0f;         ///< Time accumulator for movement updates

    // Movement settings
    float m_MovementSpeed = 250.0f;   ///< Base movement speed (blocks per second)
    float m_SprintMultiplier = 2.0f;  ///< Sprint speed multiplier
    float m_SlowMultiplier = 1.0f;    ///< Slow movement multiplier

    // Smooth movement variables
    glm::vec3 m_TargetPosition{0.0f};   ///< Target position for smooth movement
    glm::vec3 m_CurrentVelocity{0.0f};  ///< Current velocity for smooth damping
    float m_SmoothTime = 0.1f;          ///< Smoothing time factor

    // Input handling methods
    void HandleSpeedModifiers(float& speed);
    void HandleSensitivityAdjustment();
    void SmoothDamp(glm::vec3& current, const glm::vec3& target, glm::vec3& velocity,
                    float smoothTime, float deltaTime);

    // Key state tracking
    bool m_LastVPressed = false;    ///< Previous frame V key state
    bool m_LastBPressed = false;    ///< Previous frame B key state
    bool m_MovementLocked = false;  ///< Global movement lock flag
    bool m_LastEscPressed = false;  ///< Previous frame ESC key state

    // Mouse cursor control
    bool m_CursorLocked = false;  ///< Is cursor locked to window center
    void UpdateCursorState();     ///< Updates cursor visibility and lock state

    // Internal key event processing
    void OnKeyEvent(int key, int action);

    std::unordered_map<int, KeyCallback> m_KeyCallbacks;
};
}  // namespace Engine