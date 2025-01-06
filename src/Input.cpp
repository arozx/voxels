#include "Input.h"
#include <iostream>

void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_W, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_S, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_A, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_D, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_Q, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_E, deltaTime);

    // Handle sprint
    camera.SetSprinting(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    
    // Handle vertical movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_SPACE, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_LEFT_CONTROL, deltaTime);

    // Toggle ambient light with L key
    static bool lKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!lKeyPressed) {
            ambientLightEnabled = !ambientLightEnabled;
            std::cout << "Ambient light: " << (ambientLightEnabled ? "ON" : "OFF") << std::endl;
            lKeyPressed = true;
        }
    } else {
        lKeyPressed = false;
    }

    // Toggle diffuse light with K key
    static bool kKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        if (!kKeyPressed) {
            diffuseLightEnabled = !diffuseLightEnabled;
            std::cout << "Diffuse light: " << (diffuseLightEnabled ? "ON" : "OFF") << std::endl;
            kKeyPressed = true;
        }
    } else {
        kKeyPressed = false;
    }

    // Adjust diffuse strength with [ and ]
    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
        diffuseStrength = std::max(0.0f, diffuseStrength - 0.1f * deltaTime);
        std::cout << "Diffuse strength: " << diffuseStrength << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
        diffuseStrength = std::min(2.0f, diffuseStrength + 0.1f * deltaTime);
        std::cout << "Diffuse strength: " << diffuseStrength << std::endl;
    }

    // Adjust light height with + and -
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        lightHeight += 5.0f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        lightHeight = std::max(1.0f, lightHeight - 5.0f * deltaTime);
    }

    // Adjust light radius with , and .
    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
        lightRadius = std::max(1.0f, lightRadius - 5.0f * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
        lightRadius += 5.0f * deltaTime;
    }

    // Toggle specular light with J key
    static bool jKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        if (!jKeyPressed) {
            specularLightEnabled = !specularLightEnabled;
            std::cout << "Specular light: " << (specularLightEnabled ? "ON" : "OFF") << std::endl;
            jKeyPressed = true;
        }
    } else {
        jKeyPressed = false;
    }

    // Adjust specular strength with O and P
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        specularStrength = std::max(0.0f, specularStrength - 0.1f * deltaTime);
        std::cout << "Specular strength: " << specularStrength << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        specularStrength = std::min(2.0f, specularStrength + 0.1f * deltaTime);
        std::cout << "Specular strength: " << specularStrength << std::endl;
    }

    // Adjust shininess with N and M
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        shininess = std::max(1, shininess - 1);
        std::cout << "Shininess: " << shininess << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        shininess = std::min(256, shininess + 1);
        std::cout << "Shininess: " << shininess << std::endl;
    }

    // Toggle outlines with T key
    static bool tKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!tKeyPressed) {
            showOutlines = !showOutlines;
            std::cout << "Outlines: " << (showOutlines ? "ON" : "OFF") << std::endl;
            tKeyPressed = true;
        }
    } else {
        tKeyPressed = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}