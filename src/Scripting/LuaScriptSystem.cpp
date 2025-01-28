#include "LuaScriptSystem.h"

#include <pch.h>

#include "../Application.h"
#include "../Core/FileSystem.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"

namespace Engine {
LuaScriptSystem::LuaScriptSystem() : m_LuaState(std::make_unique<sol::state>()) {
    m_LuaState->open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string,
                               sol::lib::table, sol::lib::io, sol::lib::os);
}

LuaScriptSystem::~LuaScriptSystem() = default;

void LuaScriptSystem::Initialize() { RegisterEngineAPI(); }

void LuaScriptSystem::RegisterEngineAPI() {
    auto engine = m_LuaState->create_named_table("engine");

    // Terrain API
    engine.set_function("setTerrainHeight", [this](float height) {
        LOG_TRACE_CONCAT("[Lua] setTerrainHeight called with height = ", height);
        auto activeScene = Engine::SceneManager::Get().GetActiveScene();
        if (activeScene) {
            auto* terrain = activeScene->GetTerrainSystem();
            if (terrain) {
                terrain->SetBaseHeight(height);
            } else {
                LOG_ERROR("No terrain system in active scene");
            }
        } else {
            LOG_ERROR("No active scene to set terrain height");
        }
    });

    engine.set_function("generateTerrainMesh", [this](uint32_t seed) {
        auto activeScene = Engine::SceneManager::Get().GetActiveScene();
        if (activeScene) {
            auto* terrain = activeScene->GetTerrainSystem();
            if (terrain) {
                terrain->GenerateMesh(seed);
            } else {
                LOG_ERROR("No terrain system in active scene");
            }
        } else {
            LOG_ERROR("No active scene to generate terrain");
        }
        // Logging for mesh generation already handled in TerrainSystem
    });

    // Renderer API
    engine.set_function("setClearColor", [this](float r, float g, float b, float a) {
        Renderer::Get().Clear(glm::vec4(r, g, b, a));
    });

    engine.set_function("setViewport", [this](int x, int y, int width, int height) {
        Renderer::Get().SetViewport(x, y, width, height);
    });

    engine.set_function("setCameraType", [this](const std::string& type) {
        if (type == "orthographic") {
            Renderer::Get().SetCameraType(Renderer::CameraType::Orthographic);
        } else if (type == "perspective") {
            Renderer::Get().SetCameraType(Renderer::CameraType::Perspective);
        }
    });

    engine.set_function("getCameraType", []() -> std::string {
        auto type = Renderer::Get().GetCameraType();
        return (type == Renderer::CameraType::Orthographic) ? "orthographic" : "perspective";
    });

    // Scene API
    engine.set_function("createScene", [](const std::string& name) -> bool {
        LOG_TRACE_CONCAT("[Lua] createScene called with name = ", name);
        auto scene = std::make_shared<Scene>(name);
        if (!scene) {
            LOG_ERROR_CONCAT("Failed to create scene: ", name);
            return false;
        }
        SceneManager::Get().AddScene(scene);
        // Logging already handled in Scene constructor
        return true;
    });

    engine.set_function("setActiveScene", [](const std::string& name) -> bool {
        auto& sceneManager = SceneManager::Get();
        auto scene = sceneManager.GetScene(name);
        if (!scene) {
            LOG_ERROR_CONCAT("Scene not found: ", name);
            return false;
        }
        sceneManager.SetActiveScene(name);
        // Logging already handled in SceneManager
        return true;
    });

    engine.set_function("deleteScene", [](const std::string& name) -> bool {
        auto& sceneManager = SceneManager::Get();
        if (sceneManager.RemoveScene(name)) {
            LOG_INFO_CONCAT("Deleted scene: ", name);
            return true;
        }
        LOG_ERROR_CONCAT("Failed to delete scene: ", name);
        return false;
    });

    engine.set_function("getActiveSceneName", []() -> std::string {
        auto scene = SceneManager::Get().GetActiveScene();
        return scene ? scene->GetName() : "";
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

    engine.set_function("toggleCameraControls",
                        []() { Application::Get().GetInputSystem()->ToggleCameraControls(); });

    engine.set_function("toggleMovementLock",
                        []() { Application::Get().GetInputSystem()->ToggleMovementLock(); });

    engine.set_function("toggleSmoothCamera",
                        []() { Application::Get().GetInputSystem()->ToggleSmoothCamera(); });

    // Debug/Logging API
    engine.set_function(
        "trace", [](const std::string& message) { LOG_TRACE_CONCAT("[Lua]: ", message, "."); });

    engine.set_function(
        "log", [](const std::string& message) { LOG_INFO_CONCAT("[Lua]: ", message, "."); });

    engine.set_function(
        "warn", [](const std::string& message) { LOG_WARN_CONCAT("[Lua]: ", message, "."); });

    engine.set_function(
        "error", [](const std::string& message) { LOG_ERROR_CONCAT("[Lua]: ", message, "."); });

    engine.set_function(
        "fatal", [](const std::string& message) { LOG_FATAL_CONCAT("[Lua]: ", message, "."); });

    // Profiling
    engine.set_function("profileFunction", []() { PROFILE_FUNCTION(); });

    engine.set_function("profileScope", [](const std::string& name) { PROFILE_SCOPE(name); });

    // File System API
    engine.set_function("loadScript",
                        [this](const std::string& filepath) { return ExecuteFile(filepath); });

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

    engine.set_function(
        "rotateCameraWithMouse", [](float xOffset, float yOffset, float sensitivity) {
            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->RotateWithMouse(xOffset, yOffset, sensitivity);
            }
        });

    // ImGui Overlay controls
    engine.set_function("showTransformControls", [](bool show) {
        if (auto* overlay = Application::Get().GetImGuiOverlay()) {
            overlay->ShowTransformControls(show);
        }
    });

    engine.set_function("showProfiler", [](bool show) {
        if (auto* overlay = Application::Get().GetImGuiOverlay()) {
            overlay->ShowProfiler(show);
        }
    });

    engine.set_function("showRendererSettings", [](bool show) {
        if (auto* overlay = Application::Get().GetImGuiOverlay()) {
            overlay->ShowRendererSettings(show);
        }
    });

    engine.set_function("showEventDebugger", [](bool show) {
        if (auto* overlay = Application::Get().GetImGuiOverlay()) {
            overlay->ShowEventDebugger(show);
        }
    });

    engine.set_function("showTerrainControls", [](bool show) {
        if (auto* overlay = Application::Get().GetImGuiOverlay()) {
            overlay->ShowTerrainControls(show);
        }
    });

    engine.set_function("showFPSCounter", [](bool show) {
        if (auto* overlay = Application::Get().GetImGuiOverlay()) {
            overlay->ShowFPSCounter(show);
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
    sol::load_result loadResult = m_LuaState->load(script);
    if (!loadResult.valid()) {
        sol::error err = loadResult;
        LOG_ERROR_CONCAT("Failed to load script: ", err.what());
        return false;
    }

    sol::protected_function_result result = loadResult();
    if (!result.valid()) {
        sol::error err = result;
        LOG_ERROR_CONCAT("Failed to execute script: ", err.what());
        return false;
    }

    return true;
}

bool LuaScriptSystem::ExecuteFile(const std::string& originalPath) {
    std::vector<std::string> searchPaths = {
        originalPath, "../" + originalPath, "../../" + originalPath,
        "../sandbox/assets/scripts/" + originalPath.substr(originalPath.find_last_of("/\\") + 1),
        "sandbox/assets/scripts/" + originalPath.substr(originalPath.find_last_of("/\\") + 1)};

    std::string validPath;
    bool found = false;

    for (const auto& path : searchPaths) {
        if (FileSystem::Exists(path)) {
            validPath = path;
            found = true;
            break;
        }
    }

    if (!found) {
        LOG_ERROR("Script not found: ", originalPath);
        return false;
    }

    sol::load_result loadResult = m_LuaState->load_file(validPath);
    if (!loadResult.valid() || !loadResult().valid()) {
        LOG_ERROR("Failed to execute script: ", validPath);
        return false;
    }

    return true;
}
}  // namespace Engine