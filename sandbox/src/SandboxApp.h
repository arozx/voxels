#pragma once

#include "Application.h"
#include "EntryPoint.h"
#include "TerrainSystem/TerrainSystem.h"

class SandboxApp : public Engine::Application {
public:
    SandboxApp();
    virtual void OnImGuiRender() override;

private:
 Engine::TerrainSystem* m_TerrainSystem = nullptr;
 bool m_ShowConsole = false;
 std::string m_CommandBuffer;
 std::vector<std::string> m_CommandHistory;
 int m_HistoryIndex = -1;

 void ExecuteCommand(const std::string& command);
 void HandleConsoleInput();
};
