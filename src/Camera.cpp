#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
      BaseMovementSpeed(2.5f),
      SprintMultiplier(2.0f),
      CurrentSpeed(2.5f),
      IsSprinting(false),
      MouseSensitivity(0.1f), 
      Zoom(45.0f)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(int direction, float deltaTime)
{
    float velocity = CurrentSpeed * (IsSprinting ? SprintMultiplier : 1.0f) * deltaTime;
    
    glm::vec3 movement(0.0f);
    
    if (direction == GLFW_KEY_W)
        movement += Front;
    if (direction == GLFW_KEY_S)
        movement -= Front;
    if (direction == GLFW_KEY_A)
        movement -= Right;
    if (direction == GLFW_KEY_D)
        movement += Right;
    if (direction == GLFW_KEY_SPACE)
        movement += WorldUp;
    if (direction == GLFW_KEY_LEFT_CONTROL)
        movement -= WorldUp;
    if (direction == GLFW_KEY_Q)
        RollCamera(90.0f * deltaTime);
    if (direction == GLFW_KEY_E)
        RollCamera(-90.0f * deltaTime);

    // Normalize diagonal movement
    if (glm::length(movement) > 0)
        movement = glm::normalize(movement);
        
    Position += movement * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::RollCamera(float angle) {
    // Rotate around WorldUp instead of Front
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), WorldUp);
    
    // Rotate both Front and Up vectors
    Front = glm::vec3(rotation * glm::vec4(Front, 0.0f));
    Up = glm::vec3(rotation * glm::vec4(Up, 0.0f));
    
    // Recalculate Right vector
    Right = glm::normalize(glm::cross(Front, Up));
    Roll += angle;
}

void Camera::ProcessMouseScroll(float yoffset)
{
    CurrentSpeed = glm::clamp(CurrentSpeed + yoffset * SPEED_INCREMENT, MIN_SPEED, MAX_SPEED);
}

void Camera::SetSprinting(bool sprinting)
{
    IsSprinting = sprinting;
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}