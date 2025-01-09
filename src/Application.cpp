#include "Application.h"
#include "Logging.h"

namespace Engine {
    Application::Application() {
        LOG_INFO("Application Created");
    }

    Application::~Application() {
        LOG_INFO("Application Destroyed");
    }

    void Application::Run() {
        LOG_INFO("Application Starting...");
        while (true);  // Basic game loop - will be expanded later
    }
}