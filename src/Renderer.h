#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Renderer {
public:
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    static Renderer& getInstance();

    void setViewPort(int width, int height);
    void enableDepthTesting();
    void enableFaceCulling();
    void enableMSAA();
    void offScreenMSAA();
    void setRenderVersion(int major, int minor);
    void initGLFW();
    void initGLAD();

private:
    Renderer() = default;
    ~Renderer() = default;
};

#endif
