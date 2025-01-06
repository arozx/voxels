#include "Renderer.h"

// Static method to access the singleton instance
Renderer& Renderer::getInstance() {
    static Renderer instance;
    return instance;
}

void Renderer::setViewPort(int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::enableDepthTesting() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void Renderer::enableFaceCulling() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::enableMSAA() {
    glEnable(GL_MULTISAMPLE);
    glfwWindowHint(GLFW_SAMPLES, 4);
}

void Renderer::offScreenMSAA() {
    // TODO: add support for textures

    // TODO: add support for render buffer objects
}

void Renderer::setRenderVersion(int major, int minor) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Renderer::initGLFW() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
}

void Renderer::initGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}

void Renderer::terminate() {
    glfwTerminate();
}

// take text renderer as a parameter
void Renderer::displayFPS() {
    static double lastTime = getCurrentTime();
    static int nbFrames = 0;
    double currentTime = getCurrentTime();
    nbFrames++;
    if (currentTime - lastTime >= 1000.0) { // If last prinf() was more than 1 sec ago
        // printf and reset timer
        printf("%f ms/frame\n", 1000.0 / double(nbFrames));
        printf("%f FPS\n", double(nbFrames));
        nbFrames = 0;
        lastTime += 1000.0;
    }

    // Render FPS counter
    // std::string fpsText = "FPS: " + std::to_string(nbFrames);
    // textRenderer->RenderText(fpsText, 10.0f, 580.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}
