#include "SandboxApp.h"
#include <imgui.h>
#include <fstream>
#include <sstream>

SandboxApp::SandboxApp() : Application() {
    // Check script system first
    auto* scriptSystem = GetScriptSystem();
    if (!scriptSystem) {
        LOG_ERROR("Script system not available!");
        return;
    }

    // Initialize terrain after script system is ready
    m_TerrainSystem = std::make_unique<Engine::TerrainSystem>();
    if (!m_TerrainSystem) {
        LOG_ERROR("Failed to create terrain system!");
        return;
    }

    m_TerrainSystem->Initialize(GetRenderer());

    // Register console toggle
    if (auto* inputSystem = GetInputSystem()) {
        inputSystem->RegisterKeyCallback(GLFW_KEY_GRAVE_ACCENT, [this](int action) {
            if (action == GLFW_PRESS) {
                m_ShowConsole = !m_ShowConsole;
            }
        });
    }

    // Initialize with init.lua script
    try {
        std::string scriptPath = "build/assets/scripts/init.lua";
        std::ifstream scriptFile(scriptPath);
        if (!scriptFile.is_open()) {
            LOG_ERROR_CONCAT("Failed to open init.lua at path: ", scriptPath);
            return;
        }

        std::stringstream scriptBuffer;
        scriptBuffer << scriptFile.rdbuf();
        
        if (!scriptSystem->ExecuteScript(scriptBuffer.str())) {
            LOG_ERROR("Failed to execute initialization script");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Script initialization error: {", e.what(), "}");
    }
}

void SandboxApp::OnImGuiRender() {
    if (!m_ShowConsole) return;

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Lua Console", &m_ShowConsole)) {
        // Command history
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false);
        for (const auto& cmd : m_CommandHistory) {
            ImGui::TextWrapped("%s", cmd.c_str());
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        // Command input
        ImGui::Separator();
        static char inputBuffer[256];
        bool reclaimFocus = false;
        ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;

        if (ImGui::InputText("Command", inputBuffer, sizeof(inputBuffer), inputFlags)) {
            m_CommandBuffer = inputBuffer;
            ExecuteCommand(m_CommandBuffer);
            inputBuffer[0] = '\0'; // Clear the buffer
            m_CommandBuffer.clear();
            reclaimFocus = true;
        }

        if (reclaimFocus) {
            ImGui::SetKeyboardFocusHere(-1);
        }
    }
    ImGui::End();
}

void SandboxApp::ExecuteCommand(const std::string& command) {
    if (command.empty()) return;

    m_CommandHistory.push_back("> " + command);
    
    auto* scriptSystem = GetScriptSystem();
    if (!scriptSystem) {
        m_CommandHistory.push_back("Error: Script system not available");
        return;
    }

    try {
        bool success = scriptSystem->ExecuteScript(command);
        if (!success) {
            m_CommandHistory.push_back("Error: Script execution failed");
        }
    } catch (const std::exception& e) {
        m_CommandHistory.push_back(std::string("Error: ") + e.what());
    }

    m_HistoryIndex = -1;
}

namespace Engine {
    Application* CreateApplication() {
        return new SandboxApp();
    }
}
