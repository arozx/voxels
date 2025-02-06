#pragma once

#include "Application.h"
#include "EntryPoint.h"

class SandboxApp : public Engine::Application {
public:
    SandboxApp();
    virtual void OnImGuiRender() override;

private:
 // 2D Rendering members
 std::shared_ptr<Engine::OrthographicCamera> m_Camera;

 // Console members
 bool m_ShowConsole = false;
 std::string m_CommandBuffer;
 std::vector<std::string> m_CommandHistory;
 int m_HistoryIndex = -1;

 void ExecuteCommand(const std::string& command);
};
