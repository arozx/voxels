#include <pch.h>
#include "SandboxApp.h"
#include <imgui.h>


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

    // Run main.lua script
    try {
        std::string scriptPath = "build/assets/scripts/main.lua";
        std::ifstream scriptFile(scriptPath);
        if (!scriptFile.is_open()) {
            LOG_ERROR_CONCAT("Failed to open main.lua at path: ", scriptPath);
            return;
        }

        std::stringstream scriptBuffer;
        scriptBuffer << scriptFile.rdbuf();
        
        if (!scriptSystem->ExecuteScript(scriptBuffer.str())) {
            LOG_ERROR("Failed to execute main script");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Script initialization error: {", e.what(), "}");
    }
}

void SandboxApp::OnImGuiRender() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Lua Console", &m_ShowConsole)) {
        // Command history
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false);
        static std::string consoleOutput;
        consoleOutput.clear();
        for (const auto& cmd : m_CommandHistory) {
            consoleOutput += cmd + "\n";
        }
        ImGui::InputTextMultiline("##ConsoleOutput", &consoleOutput[0], consoleOutput.size() + 1, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_ReadOnly);
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        // Command input
        ImGui::Separator();
        static char inputBuffer[256];
        bool reclaimFocus = false;
        ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;

        // Handle up arrow key for command history navigation
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) && !m_CommandHistory.empty()) {
            if (m_HistoryIndex == -1) {
                m_HistoryIndex = static_cast<int>(m_CommandHistory.size()) - 1;
            }
            else if (m_HistoryIndex > 0) {
                m_HistoryIndex--;
            }
            if (m_CommandHistory[m_HistoryIndex].find("Error") == std::string::npos) {
                std::string command = m_CommandHistory[m_HistoryIndex].substr(2); // Strip the "> " prefix
                strncpy(inputBuffer, command.c_str(), sizeof(inputBuffer));
                inputBuffer[sizeof(inputBuffer) - 1] = '\0'; // Ensure null-termination
            }
            else {
                inputBuffer[0] = '\0'; // Clear the buffer if the command contains "Error"
            }
        }

        // Handle down arrow key for command history navigation
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) && !m_CommandHistory.empty() && inputBuffer[0] != '\0') {
            if (m_HistoryIndex == -1) {
                m_HistoryIndex = static_cast<int>(m_CommandHistory.size()) - 1;
            }
            else if (m_HistoryIndex < static_cast<int>(m_CommandHistory.size()) - 1) {
                m_HistoryIndex++;
            }
            else {
                m_HistoryIndex = -1;
                inputBuffer[0] = '\0'; // Clear the buffer if at the end of history
            }
            if (m_HistoryIndex != -1 && m_CommandHistory[m_HistoryIndex].find("Error") == std::string::npos) {
                std::string command = m_CommandHistory[m_HistoryIndex].substr(2); // Strip the "> " prefix
                strncpy(inputBuffer, command.c_str(), sizeof(inputBuffer));
                inputBuffer[sizeof(inputBuffer) - 1] = '\0'; // Ensure null-termination
            }
            else {
                inputBuffer[0] = '\0'; // Clear the buffer if the command contains "Error" or at the end of history
            }
        }

        if (ImGui::InputText("Command", inputBuffer, sizeof(inputBuffer), inputFlags)) {
            std::string command = inputBuffer;
            m_CommandBuffer = inputBuffer;
            ExecuteCommand(m_CommandBuffer);
            inputBuffer[0] = '\0'; // Clear the buffer
            m_CommandBuffer.clear();
            reclaimFocus = true;
            m_HistoryIndex = -1; // Reset history index after executing a command
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
            m_CommandHistory.push_back("Error: command execution failed");
        }
    }
    catch (const std::exception& e) {
        m_CommandHistory.push_back(std::string("Error: ") + e.what());
    }

    m_HistoryIndex = -1;
}

namespace Engine {
    Application* CreateApplication() {
        return new SandboxApp();
    }
}
