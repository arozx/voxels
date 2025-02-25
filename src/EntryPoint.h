#pragma once

/**
 * @file EntryPoint.h
 * @brief Application entry point definition
 * @details Defines the interface for creating the main application instance
 */

#include "Application.h"
#include <exception>
#include <iostream>

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv) {
    try {
        // Create application instance
        auto app = Engine::CreateApplication();
        if (!app) {
            throw std::runtime_error("Failed to create application instance");
        }

        // Run the application
        app->Run();

        // Clean up
        delete app;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return -1;
    }
}