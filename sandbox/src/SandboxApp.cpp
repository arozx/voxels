#include "SandboxApp.h"

#include <imgui.h>
#include <pch.h>

#include "../src/Scene/SceneManager.h"
#include "../src/Scripting/LuaScriptSystem.h"
#include "Camera/OrthographicCamera.h"
#include "Renderer/Renderer2D.h"

// Add these constants before SandboxApp constructor
static constexpr ImVec2 INITIAL_WINDOW_SIZE(520, 600);
static constexpr size_t MAX_INPUT_BUFFER_SIZE = 256;
static constexpr size_t MAX_COMMAND_HISTORY = 100;

/**
 * @brief Constructor for the SandboxApp, initializing core systems and setting up the Lua console.
 * 
 * Performs the following initialization steps:
 * - Validates the availability of the Lua script system
 * - Retrieves the active scene from the SceneManager
 * - Obtains the terrain system from the active scene
 * - Initializes the terrain system with the current renderer
 * - Registers a key callback to toggle the console display using the grave accent key
 * 
 * @note Logs error messages if any critical system initialization fails
 * @note Requires an active scene with a valid terrain system
 * @throws None Initialization failures are logged but do not throw exceptions
 */
SandboxApp::SandboxApp() : Application() {
    // Create script system first
    Engine::LuaScriptSystem* scriptSystem = GetScriptSystem();
    if (!scriptSystem) {
        LOG_ERROR("Script system not available!");
        return;
    }

    // Execute main.lua which will set up rendering type
    if (!scriptSystem->ExecuteFile("main.lua")) {
        LOG_ERROR("Failed to execute main.lua!");
        return;
    }

    // Register console toggle
    if (auto* inputSystem = GetInputSystem()) {
        inputSystem->RegisterKeyCallback(GLFW_KEY_GRAVE_ACCENT, [this](int action) {
            if (action == GLFW_PRESS) {
                m_ShowConsole = !m_ShowConsole;
            }
        });
    }
}

void SandboxApp::OnImGuiRender() {
    ImGui::SetNextWindowSize(INITIAL_WINDOW_SIZE, ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Lua Console", &m_ShowConsole)) {
        // Command history
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false);
        static std::string consoleOutput;
        consoleOutput.clear();
        // Limit command history size
        while (m_CommandHistory.size() > MAX_COMMAND_HISTORY) {
            m_CommandHistory.erase(m_CommandHistory.begin());
        }
        for (const auto& cmd : m_CommandHistory) {
            consoleOutput += cmd + "\n";
        }
        ImGui::InputTextMultiline("##ConsoleOutput", &consoleOutput[0], consoleOutput.size() + 1,
                                  ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16),
                                  ImGuiInputTextFlags_ReadOnly);
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        // Command input
        ImGui::Separator();
        static char inputBuffer[MAX_INPUT_BUFFER_SIZE];
        bool reclaimFocus = false;
        ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;

        // Handle up arrow key for command history navigation
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) && !m_CommandHistory.empty()) {
            if (m_HistoryIndex == -1) {
                m_HistoryIndex = static_cast<int>(m_CommandHistory.size()) - 1;
            } else if (m_HistoryIndex > 0 && m_HistoryIndex < static_cast<int>(m_CommandHistory.size())) {
                m_HistoryIndex--;
            }
            if (m_HistoryIndex >= 0 && m_HistoryIndex < static_cast<int>(m_CommandHistory.size()) &&
                m_CommandHistory[m_HistoryIndex].find("Error") == std::string::npos) {
                std::string command =
                    m_CommandHistory[m_HistoryIndex].substr(2);  // Strip the "> " prefix
                strncpy(inputBuffer, command.c_str(), MAX_INPUT_BUFFER_SIZE - 1);
                inputBuffer[sizeof(inputBuffer) - 1] = '\0';  // Ensure null-termination
            } else {
                inputBuffer[0] = '\0';  // Clear the buffer if the command contains "Error"
            }
        }

        // Handle down arrow key for command history navigation
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) && !m_CommandHistory.empty() &&
            inputBuffer[0] != '\0') {
            if (m_HistoryIndex == -1) {
                m_HistoryIndex = static_cast<int>(m_CommandHistory.size()) - 1;
            } else if (m_HistoryIndex < static_cast<int>(m_CommandHistory.size()) - 1) {
                m_HistoryIndex++;
            } else {
                m_HistoryIndex = -1;
                inputBuffer[0] = '\0';  // Clear the buffer if at the end of history
            }
            if (m_HistoryIndex != -1 &&
                m_CommandHistory[m_HistoryIndex].find("Error") == std::string::npos) {
                std::string command =
                    m_CommandHistory[m_HistoryIndex].substr(2);  // Strip the "> " prefix
                strncpy(inputBuffer, command.c_str(), sizeof(inputBuffer));
                inputBuffer[sizeof(inputBuffer) - 1] = '\0';  // Ensure null-termination
            } else {
                inputBuffer[0] = '\0';  // Clear the buffer if the command contains "Error" or at
                                        // the end of history
            }
        }

        if (ImGui::InputText("Command", inputBuffer, sizeof(inputBuffer), inputFlags)) {
            std::string command = inputBuffer;
            m_CommandBuffer = inputBuffer;
            ExecuteCommand(m_CommandBuffer);
            inputBuffer[0] = '\0';  // Clear the buffer
            m_CommandBuffer.clear();
            reclaimFocus = true;
            m_HistoryIndex = -1;  // Reset history index after executing a command
        }

        if (reclaimFocus) {
            ImGui::SetKeyboardFocusHere(-1);
        }
    }
    ImGui::End();
}

/**
 * @brief Executes a Lua command in the sandbox application.
 *
 * This method processes a given command by adding it to the command history,
 * retrieving the Lua script system, and attempting to execute the command.
 *
 * @param command The Lua command string to be executed.
 *
 * @details
 * - Ignores empty commands
 * - Adds the command to command history with a '>' prefix
 * - Retrieves the Lua script system
 * - Attempts to execute the command using the script system
 * - Handles potential execution failures and exceptions
 * - Logs error messages in the command history if execution fails
 * - Resets the command history index after execution
 *
 * @note If the script system is unavailable, an error is logged to the command history.
 * @note Exceptions during script execution are caught and their messages are logged.
 */
void SandboxApp::ExecuteCommand(const std::string& command) {
    if (command.empty()) return;

    m_CommandHistory.push_back("> " + command);

    Engine::LuaScriptSystem* scriptSystem = GetScriptSystem();
    if (!scriptSystem) {
        m_CommandHistory.push_back("Error: Script system not available");
        return;
    }

    try {
        bool success = scriptSystem->ExecuteScript(command);
        if (!success) {
            m_CommandHistory.push_back("Error: command execution failed");
        }
    } catch (const std::exception& e) {
        m_CommandHistory.push_back(std::string("Error: ") + e.what());
    }

    m_HistoryIndex = -1;
}

namespace Engine {
/**
 * @brief Creates and returns a new SandboxApp instance.
 * 
 * This function is responsible for instantiating the primary application 
 * for the sandbox environment. It allocates a new SandboxApp object 
 * which provides a Lua scripting console and other sandbox-specific 
 * application features.
 * 
 * @return Application* A pointer to a newly created SandboxApp instance.
 * @note The caller is responsible for managing the memory of the returned Application pointer.
 */
Application* CreateApplication() { return new SandboxApp(); }
}  // namespace Engine
