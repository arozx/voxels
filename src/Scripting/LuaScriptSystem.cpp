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

        engine.set_function("setViewport", [](int x, int y, int width, int height) {
            Application::Get().GetRenderer().SetViewport(x, y, width, height);
        });

        engine.set_function("setCameraType", [](const std::string& type) {
            auto& renderer = Application::Get().GetRenderer();
            if (type == "orthographic") {
                renderer.SetCameraType(Renderer::CameraType::Orthographic);
            } else if (type == "perspective") {
                renderer.SetCameraType(Renderer::CameraType::Perspective);
            }
        });

        engine.set_function("getCameraType", []() -> std::string {
            auto& renderer = Application::Get().GetRenderer();
            return renderer.GetCameraType() == Renderer::CameraType::Orthographic ? 
                "orthographic" : "perspective";
        });

        // Input API
        engine.set_function("isKeyPressed", [](int keycode) {
            return Application::Get().GetInputSystem()->IsKeyPressed(keycode);
        });

        engine.set_function("isMouseButtonPressed", [](int button) {
            return Application::Get().GetInputSystem()->IsMouseButtonPressed(button);
        });

        engine.set_function("getMousePosition", []() {
            auto [x, y] = Application::Get().GetInputSystem()->GetMousePosition();
            return std::make_tuple(x, y);
        });

        engine.set_function("setMouseSensitivity", [](float sensitivity) {
            Application::Get().GetInputSystem()->SetSensitivity(sensitivity);
        });

        engine.set_function("getMouseSensitivity", []() -> float {
            return Application::Get().GetInputSystem()->GetSensitivity();
        });

        engine.set_function("setMovementSpeed", [](float speed) {
            Application::Get().GetInputSystem()->SetMovementSpeed(speed);
        });

        engine.set_function("getMovementSpeed", []() -> float {
            return Application::Get().GetInputSystem()->GetMovementSpeed();
        });

        engine.set_function("toggleCameraControls", []() {
            Application::Get().GetInputSystem()->ToggleCameraControls();
        });

        engine.set_function("toggleMovementLock", []() {
            Application::Get().GetInputSystem()->ToggleMovementLock();
        });

        engine.set_function("toggleSmoothCamera", []() {
            Application::Get().GetInputSystem()->ToggleSmoothCamera();
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
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->SetPosition({x, y, z});
            } else {
                renderer.GetCamera()->SetPosition({x, y, z});
            }
        });

        engine.set_function("setCameraRotation", [](float pitch, float yaw) {
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->SetRotation(pitch, yaw);
            };
            // Note: Orthographic camera only supports Z rotation
        });

        engine.set_function("getCameraPosition", []() -> std::tuple<float, float, float> {
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                auto pos = renderer.GetPerspectiveCamera()->GetPosition();
                return std::make_tuple(pos.x, pos.y, pos.z);
            } else {
                auto pos = renderer.GetCamera()->GetPosition();
                return std::make_tuple(pos.x, pos.y, pos.z);
            }
        });

        engine.set_function("moveCameraForward", [](float deltaTime) {
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->MoveForward(deltaTime);
            }
        });

        engine.set_function("moveCameraBackward", [](float deltaTime) {
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->MoveBackward(deltaTime);
            }
        });

        engine.set_function("moveCameraLeft", [](float deltaTime) {
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->MoveLeft(deltaTime);
            }
        });

        engine.set_function("moveCameraRight", [](float deltaTime) {
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->MoveRight(deltaTime);
            }
        });

        engine.set_function("moveCameraUp", [](float deltaTime) {
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->MoveUp(deltaTime);
            }
        });

        engine.set_function("moveCameraDown", [](float deltaTime) {
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->MoveDown(deltaTime);
            }
        });

        engine.set_function("rotateCameraWithMouse", [](float xOffset, float yOffset, float sensitivity) {
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->RotateWithMouse(xOffset, yOffset, sensitivity);
            }
        });

        // ImGui Overlay controls
        

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