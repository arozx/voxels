#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float Roll;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Sprinting
    float BaseMovementSpeed;
    float SprintMultiplier;
    float CurrentSpeed;
    bool IsSprinting;

    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    glm::mat4 GetViewMatrix();

    void ProcessKeyboard(int key, float deltaTime);

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    void ProcessMouseScroll(float yoffset);
    void SetSprinting(bool sprinting);

    void RollCamera(float angle); // Add roll method

private:
    void updateCameraVectors();

    const float MIN_SPEED = 1.0f;
    const float MAX_SPEED = 10.0f;
    const float SPEED_INCREMENT = 0.5f;
};

#endif