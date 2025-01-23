#pragma once

/**
 * @file EntryPoint.h
 * @brief Application entry point definition
 * @details Defines the interface for creating the main application instance
 */

#include "Application.h"

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv) {
    auto app = Engine::CreateApplication();
    app->Run();
    delete app;
    return 0;
}