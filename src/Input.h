#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include "Camera.h"

extern Camera camera;
extern float lastX, lastY;
extern bool firstMouse;
extern bool ambientLightEnabled;
extern bool diffuseLightEnabled;
extern float diffuseStrength;
extern float lightHeight;
extern float lightRadius;
extern bool specularLightEnabled;
extern float specularStrength;
extern int shininess;
extern bool showOutlines;

void processInput(GLFWwindow* window, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#endif