#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include "Camera.h"

extern Camera camera;
extern float lastX, lastY;
extern bool firstMouse;

void processInput(GLFWwindow* window, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

#endif