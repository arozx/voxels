#include <pch.h>
#include "LuaScriptSystem.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"
#include "../Application.h"
#include "../Core/FileSystem.h"

namespace Engine {
    LuaScriptSystem::LuaScriptSystem() {
        m_LuaState = std::make_unique<sol::state>();
        m_LuaState->open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::math,
            sol::lib::string,
            sol::lib::table,
            sol::lib::io
        );
    }

    LuaScriptSystem::~LuaScriptSystem() {
        m_LuaState.reset();
        LOG_INFO("Lua script system shutdown");
    }

    void LuaScriptSystem::Initialize() {
        LOG_INFO("Initializing Lua script system");
        RegisterEngineAPI();
    }

    void LuaScriptSystem::RegisterEngineAPI() {
        auto engine = m_LuaState->create_named_table("engine");

        // Terrain API
        engine.set_function("setTerrainHeight", [](float height) {
            if (auto* terrain = Application::Get().GetTerrainSystem()) {
                terrain->SetBaseHeight(height);
                LOG_INFO_CONCAT("Terrain height set to {", height, "} from Lua.");
            }
        });

        engine.set_function("generateTerrain", [](uint32_t seed) {
            if (auto* terrain = Application::Get().GetTerrainSystem()) {
                terrain->RegenerateTerrain(seed);
                LOG_INFO("Generated terrain with seed {", seed, "} from Lua.");
            }
        });

        // Renderer API
        engine.set_function("setClearColor", [](float r, float g, float b, float a) {
            Application::Get().GetRenderer().Clear({r, g, b, a});
        });

        // Input API
        engine.set_function("isKeyPressed", [](int keycode) {
            return Application::Get().GetInputSystem()->IsKeyPressed(keycode);
        });

        engine.set_function("getMousePosition", []() {
            auto [x, y] = Application::Get().GetInputSystem()->GetMousePosition();
            return std::make_tuple(x, y);  // Explicitly create tuple
        });

        // Debug/Logging API
        engine.set_function("trace", [](const std::string& message) {
            LOG_TRACE_CONCAT("[Lua]: ", message, ".");
        });

        engine.set_function("log", [](const std::string& message) {
            LOG_INFO_CONCAT("[Lua]: ", message, ".");
        });

        engine.set_function("warn", [](const std::string& message) {
            LOG_WARN_CONCAT("[Lua]: ", message, ".");
        });

        engine.set_function("error", [](const std::string& message) {
            LOG_ERROR_CONCAT("[Lua]: ", message, ".");
        });

        // Profiling
        engine.set_function("profileFunction", []() {
            PROFILE_FUNCTION();
        });

        engine.set_function("profileScope", [](const std::string& name) {
            PROFILE_SCOPE(name);
        });

        // File System API
        engine.set_function("loadScript", [this](const std::string& filepath) {
            return ExecuteFile(filepath);
        });

        // Camera
        engine.set_function("setCameraPosition", [](float x, float y, float z) {
            if (auto* camera = Application::Get().GetCamera()) {
                camera->SetPosition({x, y, z});
            }
        });

        engine.set_function("setCameraRotation", [](float pitch, float yaw) {
            if (auto* camera = Application::Get().GetCamera()) {
                camera->SetRotation(pitch, yaw);
            }
        });

        engine.set_function("getCameraPosition", []() -> std::tuple<float, float, float> {
            if (auto* camera = Application::Get().GetCamera()) {
                auto pos = camera->GetPosition();
                return {pos.x, pos.y, pos.z};
            }
            return {0.0f, 0.0f, 0.0f};
        });

        engine.set_function("moveCameraForward", [](float deltaTime) {
            if (auto* camera = Application::Get().GetCamera()) {
                camera->MoveForward(deltaTime);
            }
        });

        engine.set_function("moveCameraBackward", [](float deltaTime) {
            if (auto* camera = Application::Get().GetCamera()) {
                camera->MoveBackward(deltaTime);
            }
        });

        engine.set_function("moveCameraLeft", [](float deltaTime) {
            if (auto* camera = Application::Get().GetCamera()) {
                camera->MoveLeft(deltaTime);
            }
        });

        engine.set_function("moveCameraRight", [](float deltaTime) {
            if (auto* camera = Application::Get().GetCamera()) {
                camera->MoveRight(deltaTime);
            }
        });

        engine.set_function("moveCameraUp", [](float deltaTime) {
            if (auto* camera = Application::Get().GetCamera()) {
                camera->MoveUp(deltaTime);
            }
        });

        engine.set_function("moveCameraDown", [](float deltaTime) {
            if (auto* camera = Application::Get().GetCamera()) {
                camera->MoveDown(deltaTime);
            }
        });

        engine.set_function("rotateCameraWithMouse", [](float xOffset, float yOffset, float sensitivity) {
            if (auto* camera = Application::Get().GetCamera()) {
                camera->RotateWithMouse(xOffset, yOffset, sensitivity);
            }
        });

        // Constants
        auto keyCodes = m_LuaState->create_named_table("KeyCode");
        keyCodes["ESCAPE"] = GLFW_KEY_ESCAPE;
        keyCodes["SPACE"] = GLFW_KEY_SPACE;
        keyCodes["W"] = GLFW_KEY_W;
        keyCodes["A"] = GLFW_KEY_A;
        keyCodes["S"] = GLFW_KEY_S;
        keyCodes["D"] = GLFW_KEY_D;
    }

    bool LuaScriptSystem::ExecuteScript(const std::string& script) {
        try {
            sol::protected_function_result result = m_LuaState->safe_script(
                script,
                [](lua_State*, sol::protected_function_result pfr) {
                    return pfr;
                }
            );

            if (!result.valid()) {
                sol::error err = result;
                LOG_ERROR_CONCAT("Lua script execution failed: {", err.what(), "}");
                return false;
            }
            return true;
        }
        catch (const sol::error& e) {
            LOG_ERROR_CONCAT("Lua error: {", e.what(), "}");
            return false;
        }
    }

    bool LuaScriptSystem::ExecuteFile(const std::string& filepath) {
        if (!FileSystem::Exists(filepath)) {
            LOG_ERROR_CONCAT("Script file not found: {", filepath, "}");
            return false;
        }

        try {
            sol::protected_function_result result = m_LuaState->safe_script_file(
                filepath,
                [](lua_State*, sol::protected_function_result pfr) {
                    return pfr;
                }
            );

            if (!result.valid()) {
                sol::error err = result;
                LOG_ERROR_CONCAT("Lua script file execution failed: {", err.what(), "}");
                return false;
            }
            return true;
        }
        catch (const sol::error& e) {
            LOG_ERROR_CONCAT("Lua error in file {", filepath, "} {", e.what(), "}");
            return false;
        }
    }
}