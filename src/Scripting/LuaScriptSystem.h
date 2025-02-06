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

    // Disable copy operations due to unique_ptr member
    LuaScriptSystem(const LuaScriptSystem&) = delete;
    LuaScriptSystem& operator=(const LuaScriptSystem&) = delete;
    // Enable move operations
    LuaScriptSystem(LuaScriptSystem&&) = default;
    LuaScriptSystem& operator=(LuaScriptSystem&&) = default;

    /// @brief Initialize the Lua scripting system
    /// @return true if initialization succeeded, false otherwise
    void Initialize();

    /// @brief Execute a Lua script from a string
    /// @param script The Lua script to execute
    /// @return true if execution succeeded, false otherwise
    bool ExecuteScript(const std::string& script);

    /// @brief Execute a Lua script from a file
    /// @param filepath Path to the Lua script file
    /// @return true if execution succeeded, false otherwise
    bool ExecuteFile(const std::string& filepath);

    void CallGlobalFunction(const std::string& functionName);

   private:
    std::unique_ptr<sol::state> m_LuaState;

   private:
    void RegisterEngineAPI();
};
}