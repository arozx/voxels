#pragma once

#include <pch.h>

#include "../Core/FileWatcher.h"
#include "../external/sol2/sol.hpp"
#include "Scene/SceneManager.h"

namespace Engine {
class TerrainSystem;

class LuaScriptSystem {
   public:
    // Current API version - increment when breaking changes are made
    static constexpr int API_VERSION = 1;

    // Default script execution timeout in milliseconds (5 seconds)
    static constexpr uint32_t DEFAULT_TIMEOUT_MS = 5000;

    LuaScriptSystem();
    ~LuaScriptSystem();

    // Disable copy operations due to unique_ptr member
    LuaScriptSystem(const LuaScriptSystem&) = delete;
    LuaScriptSystem& operator=(const LuaScriptSystem&) = delete;
    // Enable move operations
    LuaScriptSystem(LuaScriptSystem&&) = default;
    LuaScriptSystem& operator=(LuaScriptSystem&&) = default;

    /**
     * @brief Initialize the Lua scripting system and register all API functions
     *
     * This must be called before any scripts can be executed.
     */
    void Initialize();

    /**
     * @brief Execute a Lua script from a string
     * @param script The Lua script to execute
     * @return true if execution succeeded, false otherwise
     */
    bool ExecuteScript(const std::string& script);

    /**
     * @brief Execute a Lua script from a string with execution timeout
     * @param script The Lua script to execute
     * @param timeoutMs Maximum execution time in milliseconds (0 for no timeout)
     * @return true if execution succeeded, false otherwise
     */
    bool ExecuteScriptWithTimeout(const std::string& script,
                                  uint32_t timeoutMs = DEFAULT_TIMEOUT_MS);

    /**
     * @brief Execute a Lua script from a file
     * @param filepath Path to the Lua script file
     * @return true if execution succeeded, false otherwise
     */
    bool ExecuteFile(const std::string& filepath);

    /**
     * @brief Execute a Lua script in a sandboxed environment
     * @param script The Lua script to execute
     * @param allowedLibs Map of libraries to allow and whether they should be unrestricted
     * @return true if execution succeeded, false otherwise
     */
    bool ExecuteScriptSandboxed(const std::string& script,
                                const std::unordered_map<std::string, bool>& allowedLibs = {
                                    {"math", true},
                                    {"table", true},
                                    {"string", true},
                                    {"os", false}  // Restricted os library
                                });

    /**
     * @brief Call a global function defined in Lua
     * @param functionName Name of the global function to call
     * @throws Nothing - errors are logged but not propagated
     */
    void CallGlobalFunction(const std::string& functionName);

    /**
     * @brief Reloads a previously loaded script file
     * @param filepath Path to the script file to reload
     * @return true if reload succeeded, false otherwise
     */
    bool ReloadFile(const std::string& filepath);

    /**
     * @brief Register a callback for when a script file is reloaded
     * @param callback Function to call when any script is reloaded
     */
    void SetReloadCallback(std::function<void(const std::string&)> callback) {
        m_ReloadCallback = callback;
    }

    /**
     * @brief Get the API version exposed to Lua scripts
     * @return The current API version number
     */
    int GetAPIVersion() const { return API_VERSION; }

   private:
    // Main registration method
    void RegisterEngineAPI();

    // Individual API registration methods
    void RegisterTypeDefinitions();
    void RegisterTerrainAPI(sol::table& engine);
    void RegisterRendererAPI(sol::table& engine);
    void RegisterSceneAPI(sol::table& engine);
    void RegisterInputAPI(sol::table& engine);
    void RegisterLoggingAPI(sol::table& engine);
    void RegisterFileSystemAPI(sol::table& engine);
    void RegisterCameraAPI(sol::table& engine);
    void RegisterUIControlsAPI(sol::table& engine);
    void RegisterConstantsAPI();
    void Register2DRendererAPI(sol::table& engine);
    void RegisterObjectManagementAPI(sol::table& engine);

    // Helper methods for common validation patterns
    /**
     * @brief Execute a function with the active scene if it exists
     * @tparam Func Function type that accepts a shared_ptr<Scene>
     * @param func Function to execute with the active scene
     * @param errorMessage Message to log if no active scene exists
     * @return Result of the function or default value if no scene exists
     */
    template <typename Func>
    auto WithActiveScene(Func&& func, const std::string& errorMessage = "No active scene")
        -> decltype(func(std::declval<std::shared_ptr<Scene>>())) {
        auto scene = SceneManager::Get().GetActiveScene();
        if (!scene) {
            // Convert std::string to const char* for LOG_ERROR
            LOG_ERROR_CONCAT(errorMessage.c_str());
            return decltype(func(scene))();
        }
        return func(scene);
    }

    /**
     * @brief Validate argument type is as expected
     * @tparam T Expected type
     * @param arg The sol::object to check
     * @param name Argument name for error messages
     * @return true if type matches, false otherwise
     */
    template <typename T>
    bool ValidateArgType(const sol::object& arg, const std::string& name) {
        if (arg.get_type() != sol::type_of<T>()) {
            LOG_ERROR_CONCAT("Expected ", name, " to be of type ", typeid(T).name());
            return false;
        }
        return true;
    }

    /**
     * @brief Creates a sandboxed environment for script execution
     * @param allowedLibs Map of libraries to allow and whether they should be restricted
     * @return A sandboxed Lua environment
     */
    sol::environment CreateSandbox(const std::unordered_map<std::string, bool>& allowedLibs);

   private:
    std::unique_ptr<sol::state> m_LuaState;
    std::unordered_set<std::string> m_LoadedScripts;  // Track loaded script paths
    FileWatcher m_ScriptWatcher;
    std::function<void(const std::string&)> m_ReloadCallback;

    // Execution time limit in milliseconds (0 means no limit)
    uint32_t m_DefaultTimeoutMs = DEFAULT_TIMEOUT_MS;
};
}