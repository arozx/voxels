#pragma once

#include <pch.h>

#include "../Core/FileWatcher.h"
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

    /// @brief Reloads a previously loaded script file
    /// @param filepath Path to the script file to reload
    /// @return true if reload succeeded, false otherwise
    bool ReloadFile(const std::string& filepath);

    /// @brief Register a callback for when a script file is reloaded
    /// @param callback Function to call when any script is reloaded
    void SetReloadCallback(std::function<void(const std::string&)> callback) {
        m_ReloadCallback = callback;
    }

   private:
    std::unique_ptr<sol::state> m_LuaState;
    std::unordered_set<std::string> m_LoadedScripts;  // Track loaded script paths
    FileWatcher m_ScriptWatcher;
    std::function<void(const std::string&)> m_ReloadCallback;

   private:
    void RegisterEngineAPI();
};
}