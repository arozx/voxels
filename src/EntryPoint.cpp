#include "Application.h"

namespace Engine {
    /**
     * @brief Creates the application instance
     * @return Pointer to newly created Application
     */
    Application* CreateApplication() {
        return new Application();
    }
}

/**
 * @brief Program entry point
 * @param argc Argument count
 * @param argv Argument values
 * @return Program exit code
 */
int main(int argc, char** argv) {
    auto app = Engine::CreateApplication();
    app->Run();
    delete app;
    return 0;
}