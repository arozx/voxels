#include "SandboxApp.h"

SandboxApp::SandboxApp() : Application() {
    // Initialize terrain system in sandbox
    m_TerrainSystem = std::make_unique<Engine::TerrainSystem>();
    m_TerrainSystem->Initialize(GetRenderer());
}

namespace Engine {
    Application* CreateApplication() {
        return new SandboxApp();
    }
}
