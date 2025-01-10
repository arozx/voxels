#include "Application.h"

namespace Engine {
    Application* CreateApplication() {
        return new Application();
    }
}

int main(int argc, char** argv) {
    auto app = Engine::CreateApplication();
    app->Run();
    delete app;
    return 0;
}