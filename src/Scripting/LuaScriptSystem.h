#pragma once

#include <pch.h>

#include "../external/sol2/sol.hpp"
#include "Scene/SceneManager.h"

namespace Engine {
class TerrainSystem;

class LuaScriptSystem {
   public:
    LuaScriptSystem();
    ~LuaScriptSystem();

   public:
    void Initialize();
    bool ExecuteScript(const std::string& script);
    bool ExecuteFile(const std::string& filepath);

   private:
    std::unique_ptr<sol::state> m_LuaState;

   private:
    void RegisterEngineAPI();
};
}