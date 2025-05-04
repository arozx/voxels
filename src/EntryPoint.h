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

/**
     * @brief Application entry point.
     *
     * This function initializes the application by obtaining an instance via Engine::CreateApplication(). 
     * If the application instance is not created (i.e., returns a null pointer), it throws a runtime error. 
     * On a successful creation, it runs the application and cleans up the allocated resources before returning 0.
     *
     * @param argc The number of command-line arguments.
     * @param argv The array of command-line argument strings.
     * @return int Returns 0 on success, or -1 if an error is encountered.
     *
     * @throws std::runtime_error if Engine::CreateApplication() returns a null pointer.
     */
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