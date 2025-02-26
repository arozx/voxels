#include "LuaScriptSystem.h"

#include <pch.h>

#include "../Application.h"
#include "../Core/FileSystem.h"
#include "../Core/Transform.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Scene/SceneObject.h"
#include "Renderer/Renderer2D.h"
#include "Shader/ShaderLibrary.h"

namespace Engine {
// Forward declare DefaultShaders
class DefaultShaders;

namespace {
constexpr float DEFAULT_ORTHO_LEFT = -1.6f;
constexpr float DEFAULT_ORTHO_RIGHT = 1.6f;
constexpr float DEFAULT_ORTHO_BOTTOM = -0.9f;
constexpr float DEFAULT_ORTHO_TOP = 0.9f;

constexpr int CHECKER_TEXTURE_SIZE = 8;
}  // namespace

/**
 * @brief Constructs a new LuaScriptSystem instance and initializes the Lua state.
 *
 * Creates a unique pointer to a Lua state and opens standard Lua libraries to prepare
 * the scripting environment. The opened libraries include:
 * - Base library (standard Lua functions)
 * - Package library (module loading)
 * - Math library (mathematical operations)
 * - String library (string manipulation)
 * - Table library (table operations)
 * - IO library (input/output operations)
 * - OS library (operating system interactions)
 *
 * @note This constructor sets up a fully functional Lua scripting environment ready
 * for engine integration and script execution.
 */
LuaScriptSystem::LuaScriptSystem() : m_LuaState(std::make_unique<sol::state>()) {
    m_LuaState->open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string,
                               sol::lib::table, sol::lib::io, sol::lib::os);

    // Create and store engine table in global state
    m_LuaState->create_named_table("engine");
}

/**
 * @brief Default destructor for the LuaScriptSystem.
 *
 * Cleans up the LuaScriptSystem instance, allowing for proper resource management
 * when the object goes out of scope or is explicitly deleted. The default implementation
 * ensures that any resources managed by the base class or compiler-generated destructors
 * are properly released.
 *
 * @note This destructor is defaulted, meaning the compiler will generate the default
 * implementation for destroying the LuaScriptSystem object.
 */
LuaScriptSystem::~LuaScriptSystem() = default;

/**
 * @brief Initializes the Lua scripting system by registering the engine's API.
 *
 * This method calls the RegisterEngineAPI() function to set up the Lua environment
 * with various engine-specific functions and APIs, enabling Lua scripts to interact
 * with different engine components such as terrain, rendering, scene management,
 * input handling, and debugging.
 *
 * @note This method must be called after creating a LuaScriptSystem instance to
 * ensure that the Lua scripting environment is properly configured and ready for use.
 */
void LuaScriptSystem::Initialize() { RegisterEngineAPI(); }

/**
 * @brief Registers the engine's API functions for Lua scripting.
 *
 * This method creates a named table 'engine' in the Lua state and populates it with
 * various functions that allow Lua scripts to interact with different engine systems.
 *
 * The registered API includes functionality for:
 * - Terrain manipulation (height setting, mesh generation)
 * - Renderer controls (clear color, viewport, camera type)
 * - Scene management (create, set active, delete scenes)
 * - Input handling (key/mouse button states, mouse position)
 * - Logging and debugging
 * - Profiling
 * - Camera control
 * - ImGui overlay controls
 *
 * @note Functions are registered as lambda functions that interact with various
 * engine systems like SceneManager, Renderer, Application, and ImGuiOverlay.
 *
 * @warning Some functions have safety checks to prevent operations on null systems
 * or inactive scenes, with appropriate error logging.
 *
 * @see LuaScriptSystem
 * @see SceneManager
 * @see Renderer
 * @see Application
 */

void LuaScriptSystem::RegisterEngineAPI() {
    sol::table engine = (*m_LuaState)["engine"].get<sol::table>();

    // Add API version to the engine table
    engine["API_VERSION"] = API_VERSION;

    // Register script watcher updater
    engine.set_function("updateScriptWatcher", [this]() { m_ScriptWatcher.Update(); });

    // Register all API components
    RegisterTypeDefinitions();
    RegisterTerrainAPI(engine);
    RegisterRendererAPI(engine);
    RegisterSceneAPI(engine);
    RegisterInputAPI(engine);
    RegisterLoggingAPI(engine);
    RegisterFileSystemAPI(engine);
    RegisterCameraAPI(engine);
    RegisterUIControlsAPI(engine);
    RegisterConstantsAPI();
    Register2DRendererAPI(engine);
    RegisterObjectManagementAPI(engine);
}

/**
 * @brief Registers C++ types for use in the Lua environment
 *
 * This method creates usertype bindings for C++ classes like glm::vec3,
 * Transform, and SceneObject, exposing their methods and properties to Lua.
 */
void LuaScriptSystem::RegisterTypeDefinitions() {
    // Register glm::vec3
    m_LuaState->new_usertype<glm::vec3>(
        "vec3", sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
        "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);

    // Register Transform with proper method overloading
    m_LuaState->new_usertype<Transform>(
        "Transform", sol::constructors<Transform()>(),

        // Position methods with proper overloading
        "SetPosition",
        sol::overload(
            static_cast<void (Transform::*)(float, float, float)>(&Transform::SetPosition),
            static_cast<void (Transform::*)(const glm::vec3&)>(&Transform::SetPosition)),

        // Rotation methods with proper overloading
        "SetRotation",
        sol::overload(
            static_cast<void (Transform::*)(float, float, float)>(&Transform::SetRotation),
            static_cast<void (Transform::*)(const glm::vec3&)>(&Transform::SetRotation)),

        // Scale methods with proper overloading
        "SetScale",
        sol::overload(static_cast<void (Transform::*)(float, float, float)>(&Transform::SetScale),
                      static_cast<void (Transform::*)(const glm::vec3&)>(&Transform::SetScale)),

        // Getter methods
        "GetPosition", &Transform::GetPosition, "GetRotation", &Transform::GetRotation, "GetScale",
        &Transform::GetScale, "GetModelMatrix", &Transform::GetModelMatrix);

    // Register SceneObject with property access
    m_LuaState->new_usertype<SceneObject>(
        "SceneObject", sol::constructors<SceneObject(), SceneObject(const std::string&)>(), "name",
        &SceneObject::name, "transform",
        sol::property([](SceneObject& obj) -> Transform& { return obj.GetTransform(); }), "SetMesh",
        &SceneObject::SetMesh, "SetMaterial", &SceneObject::SetMaterial, "GetMesh",
        &SceneObject::GetMesh, "GetMaterial", &SceneObject::GetMaterial);
}

/**
 * @brief Registers terrain manipulation functions in the Lua environment
 *
 * @param engine The engine table to add the functions to
 */
void LuaScriptSystem::RegisterTerrainAPI(sol::table& engine) {
    // Set terrain height
    engine.set_function("setTerrainHeight", [this](float height) -> bool {
        LOG_TRACE_CONCAT("[Lua] setTerrainHeight called with height = ", height);

        return this->WithActiveScene(
            [height](auto scene) -> bool {
                auto* terrain = scene->GetTerrainSystem();
                if (!terrain) {
                    LOG_ERROR_CONCAT("No terrain system in active scene");
                    return false;
                }

                terrain->SetBaseHeight(height);
                return true;
            },
            "No active scene to set terrain height");
    });

    // Generate terrain mesh
    engine.set_function("generateTerrainMesh", [this](uint32_t seed) -> bool {
        return this->WithActiveScene(
            [seed](auto scene) -> bool {
                auto* terrain = scene->GetTerrainSystem();
                if (!terrain) {
                    LOG_ERROR_CONCAT("No terrain system in active scene");
                    return false;
                }

                terrain->GenerateMesh(seed);
                return true;
            },
            "No active scene to generate terrain");
    });
}

/**
 * @brief Registers renderer control functions in the Lua environment
 *
 * @param engine The engine table to add the functions to
 */
void LuaScriptSystem::RegisterRendererAPI(sol::table& engine) {
    // Set clear color for renderer
    engine.set_function("setClearColor", [](float r, float g, float b, float a) -> bool {
        if (r < 0.0f || r > 1.0f || g < 0.0f || g > 1.0f || b < 0.0f || b > 1.0f || a < 0.0f ||
            a > 1.0f) {
            LOG_WARN("Color values should be between 0.0 and 1.0");
            return false;
        }

        Renderer::Get().Clear(glm::vec4(r, g, b, a));
        return true;
    });

    // Set viewport dimensions
    engine.set_function("setViewport", [](int x, int y, int width, int height) -> bool {
        if (width <= 0 || height <= 0) {
            LOG_ERROR_CONCAT("Viewport width and height must be positive");
            return false;
        }

        Renderer::Get().SetViewport(x, y, width, height);
        return true;
    });

    // Set camera type (orthographic or perspective)
    engine.set_function("setCameraType", [](const std::string& type) -> bool {
        if (type == "orthographic") {
            Application::Get().SetCameraType(CameraType::Orthographic);
            return true;
        } else if (type == "perspective") {
            Application::Get().SetCameraType(CameraType::Perspective);
            return true;
        } else {
            LOG_ERROR_CONCAT("Invalid camera type: ", type);
            LOG_INFO_CONCAT("Valid types are 'orthographic' or 'perspective'");
            return false;
        }
    });

    // Get current camera type
    engine.set_function("getCameraType", []() -> std::string {
        auto type = Renderer::Get().GetCameraType();
        return (type == Renderer::CameraType::Orthographic) ? "orthographic" : "perspective";
    });

    // Set render type (2D or 3D)
    engine.set_function("setRenderType", [](const std::string& type) -> bool {
        if (type == "2d") {
            Application::Get().SetRenderType(RenderType::Render2D);
            return true;
        } else if (type == "3d") {
            Application::Get().SetRenderType(RenderType::Render3D);
            return true;
        } else {
            LOG_ERROR_CONCAT("Invalid render type: ", type);
            LOG_INFO_CONCAT("Valid types are '2d' or '3d'");
            return false;
        }
    });

    // Get current render type
    engine.set_function("getRenderType", []() -> std::string {
        return Application::Get().GetRenderType() == RenderType::Render2D ? "2d" : "3d";
    });

    // Check if current render mode is 3D
    engine.set_function("is3D", []() -> bool {
        return Application::Get().GetRenderType() == RenderType::Render3D;
    });
}

/**
 * @brief Registers scene management functions in the Lua environment
 *
 * @param engine The engine table to add the functions to
 */
void LuaScriptSystem::RegisterSceneAPI(sol::table& engine) {
    // Create a new scene
    engine.set_function("createScene", [](const std::string& name) -> bool {
        if (name.empty()) {
            LOG_ERROR_CONCAT("Scene name cannot be empty");
            return false;
        }

        auto scene = std::make_shared<Scene>(name);
        if (!scene) {
            LOG_ERROR_CONCAT("Failed to create scene: ", name);
            return false;
        }
        
        SceneManager::Get().AddScene(scene);
        LOG_INFO_CONCAT("Created scene: ", name);
        return true;
    });

    // Set the active scene
    engine.set_function("setActiveScene", [](const std::string& name) -> bool {
        if (name.empty()) {
            LOG_ERROR_CONCAT("Scene name cannot be empty");
            return false;
        }

        auto& sceneManager = SceneManager::Get();
        auto scene = sceneManager.GetScene(name);
        if (!scene) {
            LOG_ERROR_CONCAT("Scene not found: ", name);
            return false;
        }
        
        sceneManager.SetActiveScene(name);
        LOG_INFO_CONCAT("Set active scene to: ", name);
        return true;
    });

    // Remove a scene
    engine.set_function("removeScene", [](const std::string& name) -> bool {
        if (name.empty()) {
            LOG_ERROR_CONCAT("Scene name cannot be empty");
            return false;
        }

        bool result = SceneManager::Get().RemoveScene(name);
        if (result) {
            LOG_INFO_CONCAT("Removed scene: ", name);
        } else {
            LOG_ERROR_CONCAT("Failed to remove scene: ", name);
        }
        return result;
    });

    // Get the active scene name
    engine.set_function("getActiveSceneName", []() -> std::string {
        auto scene = SceneManager::Get().GetActiveScene();
        return scene ? scene->GetName() : "";
    });
}

/**
 * @brief Registers input handling functions in the Lua environment
 *
 * @param engine The engine table to add the functions to
 */
void LuaScriptSystem::RegisterInputAPI(sol::table& engine) {
    // Check if a key is pressed
    engine.set_function("isKeyPressed", [](int keycode) -> bool {
        return Application::Get().GetInputSystem()->IsKeyPressed(keycode);
    });

    // Check if a mouse button is pressed
    engine.set_function("isMouseButtonPressed", [](int button) -> bool {
        if (button < 0 || button > 7) {
            LOG_WARN("Mouse button index out of range: ", button);
        }
        return Application::Get().GetInputSystem()->IsMouseButtonPressed(button);
    });

    // Get current mouse position
    engine.set_function("getMousePosition", []() -> std::tuple<float, float> {
        auto [x, y] = Application::Get().GetInputSystem()->GetMousePosition();
        return std::make_tuple(x, y);
    });

    // Set mouse sensitivity
    engine.set_function("setMouseSensitivity", [](float sensitivity) -> bool {
        if (sensitivity <= 0.0f) {
            LOG_WARN("Mouse sensitivity should be positive");
            return false;
        }
        Application::Get().GetInputSystem()->SetSensitivity(sensitivity);
        return true;
    });

    // Get mouse sensitivity
    engine.set_function("getMouseSensitivity", []() -> float {
        return Application::Get().GetInputSystem()->GetSensitivity();
    });

    // Set movement speed
    engine.set_function("setMovementSpeed", [](float speed) -> bool {
        if (speed < 0.0f) {
            LOG_WARN("Movement speed should be non-negative");
            return false;
        }
        Application::Get().GetInputSystem()->SetMovementSpeed(speed);
        return true;
    });

    // Get movement speed
    engine.set_function("getMovementSpeed", []() -> float {
        return Application::Get().GetInputSystem()->GetMovementSpeed();
    });

    // Toggle camera controls
    engine.set_function("toggleCameraControls", []() -> bool {
        Application::Get().GetInputSystem()->ToggleCameraControls();
        return true;
    });

    // Toggle movement lock
    engine.set_function("toggleMovementLock", []() -> bool {
        Application::Get().GetInputSystem()->ToggleMovementLock();
        return true;
    });

    // Toggle smooth camera
    engine.set_function("toggleSmoothCamera", []() -> bool {
        Application::Get().GetInputSystem()->ToggleSmoothCamera();
        return true;
    });
}

/**
 * @brief Registers logging and profiling functions in the Lua environment
 *
 * @param engine The engine table to add the functions to
 */
void LuaScriptSystem::RegisterLoggingAPI(sol::table& engine) {
    // Register logging functions with consistent formatting
    engine.set_function("trace", [](const std::string& message) -> bool {
        LOG_TRACE_CONCAT("[Lua]: ", message);
        return true;
    });

    engine.set_function("log", [](const std::string& message) -> bool {
        LOG_INFO_CONCAT("[Lua]: ", message);
        return true;
    });

    engine.set_function("warn", [](const std::string& message) -> bool {
        LOG_WARN_CONCAT("[Lua]: ", message);
        return true;
    });

    engine.set_function("error", [](const std::string& message) -> bool {
        LOG_ERROR_CONCAT("[Lua]: ", message);
        return true;
    });

    engine.set_function("fatal", [](const std::string& message) -> bool {
        LOG_FATAL_CONCAT("[Lua]: ", message);
        return true;
    });

    // Profiling functions
    engine.set_function("profileFunction", []() -> bool {
        PROFILE_FUNCTION();
        return true;
    });

    engine.set_function("profileScope", [](const std::string& name) -> bool {
        if (name.empty()) {
            LOG_WARN("Profile scope name should not be empty");
        }
        PROFILE_SCOPE(name);
        return true;
    });
}

/**
 * @brief Registers file system interaction functions in the Lua environment
 *
 * @param engine The engine table to add the functions to
 */
void LuaScriptSystem::RegisterFileSystemAPI(sol::table& engine) {
    // Load and execute a script file
    engine.set_function("loadScript", [this](const std::string& filepath) -> bool {
        if (filepath.empty()) {
            LOG_ERROR_CONCAT("Script filepath cannot be empty");
            return false;
        }
        return ExecuteFile(filepath);
    });

    // Create a directory
    engine.set_function("mkdir", [](const std::string& path) -> bool {
        if (path.empty()) {
            LOG_ERROR_CONCAT("Directory path cannot be empty");
            return false;
        }
        return FileSystem::CreateDirectory(path);
    });

    // Check if a file or directory exists
    engine.set_function("exists", [](const std::string& path) -> bool {
        if (path.empty()) {
            LOG_ERROR_CONCAT("Path cannot be empty");
            return false;
        }
        return FileSystem::Exists(path);
    });
}

/**
 * @brief Registers camera control functions in the Lua environment
 *
 * @param engine The engine table to add the functions to
 */
void LuaScriptSystem::RegisterCameraAPI(sol::table& engine) {
    // Set camera position
    engine.set_function("setCameraPosition", [](float x, float y, float z) -> bool {
        auto& renderer = Application::Get().GetRenderer();
        if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
            renderer.GetPerspectiveCamera()->SetPosition({x, y, z});
        } else {
            renderer.GetCamera()->SetPosition({x, y, z});
        }
        return true;
    });

    // Set camera rotation (pitch and yaw for perspective camera)
    engine.set_function("setCameraRotation", [](float pitch, float yaw) -> bool {
        auto& renderer = Application::Get().GetRenderer();
        if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
            renderer.GetPerspectiveCamera()->SetRotation(pitch, yaw);
            return true;
        }
        LOG_WARN("setCameraRotation has no effect on orthographic camera");
        return false;
    });

    // Get camera position
    engine.set_function("getCameraPosition", []() -> std::tuple<float, float, float> {
        auto& renderer = Application::Get().GetRenderer();
        if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
            const auto& pos = renderer.GetPerspectiveCamera()->GetPosition();
            return std::make_tuple(pos.x, pos.y, pos.z);
        } else {
            const auto& pos = renderer.GetCamera()->GetPosition();
            return std::make_tuple(pos.x, pos.y, pos.z);
        }
    });

    // Camera movement functions (only for perspective camera)
    auto registerCameraMovement = [&engine](const char* name, auto method) {
        // Fix: Capture the name parameter in the lambda
        engine.set_function(name, [name, method](float deltaTime) -> bool {
            if (deltaTime < 0) {
                LOG_WARN("Delta time should not be negative");
                return false;
            }

            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                // Fix: Proper way to call a member function through a shared_ptr
                auto camera = renderer.GetPerspectiveCamera();
                ((*camera).*method)(deltaTime);  // Dereference the shared_ptr and use .*
                return true;
            }
            LOG_WARN(name, " has no effect on orthographic camera");
            return false;
        });
    };

    // Register all camera movement methods
    registerCameraMovement("moveCameraForward", &PerspectiveCamera::MoveForward);
    registerCameraMovement("moveCameraBackward", &PerspectiveCamera::MoveBackward);
    registerCameraMovement("moveCameraLeft", &PerspectiveCamera::MoveLeft);
    registerCameraMovement("moveCameraRight", &PerspectiveCamera::MoveRight);
    registerCameraMovement("moveCameraUp", &PerspectiveCamera::MoveUp);
    registerCameraMovement("moveCameraDown", &PerspectiveCamera::MoveDown);

    // Camera rotation with mouse
    engine.set_function(
        "rotateCameraWithMouse", [](float xOffset, float yOffset, float sensitivity) -> bool {
            if (sensitivity <= 0.0f) {
                LOG_WARN("Camera sensitivity should be positive");
                return false;
            }

            auto& renderer = Application::Get().GetRenderer();
            if (renderer.GetCameraType() == Renderer::CameraType::Perspective) {
                renderer.GetPerspectiveCamera()->RotateWithMouse(xOffset, yOffset, sensitivity);
                return true;
            }
            LOG_WARN("rotateCameraWithMouse has no effect on orthographic camera");
            return false;
        });
}

/**
 * @brief Registers UI controls for ImGui overlay in the Lua environment
 *
 * @param engine The engine table to add the functions to
 */
void LuaScriptSystem::RegisterUIControlsAPI(sol::table& engine) {
    // Helper function to register ImGui overlay control functions
    auto registerOverlayFunction = [&engine](const char* name, auto method) {
        engine.set_function(name, [name, method](bool show) -> bool {
            auto* overlay = Application::Get().GetImGuiOverlay();
            if (!overlay) {
                LOG_WARN("ImGui overlay not available for ", name);
                return false;
            }
            (overlay->*method)(show);
            return true;
        });
    };

    // Register all overlay control functions
    registerOverlayFunction("showTransformControls", &ImGuiOverlay::ShowTransformControls);
    registerOverlayFunction("showProfiler", &ImGuiOverlay::ShowProfiler);
    registerOverlayFunction("showRendererSettings", &ImGuiOverlay::ShowRendererSettings);
    registerOverlayFunction("showEventDebugger", &ImGuiOverlay::ShowEventDebugger);
    registerOverlayFunction("showTerrainControls", &ImGuiOverlay::ShowTerrainControls);
    registerOverlayFunction("showFPSCounter", &ImGuiOverlay::ShowFPSCounter);
}

/**
 * @brief Registers constants and enums in the Lua environment
 */
void LuaScriptSystem::RegisterConstantsAPI() {
    // Register key code constants
    auto keyCodes = m_LuaState->create_named_table("KeyCode");

    // Common keyboard keys
    keyCodes["ESCAPE"] = GLFW_KEY_ESCAPE;
    keyCodes["SPACE"] = GLFW_KEY_SPACE;
    keyCodes["W"] = GLFW_KEY_W;
    keyCodes["A"] = GLFW_KEY_A;
    keyCodes["S"] = GLFW_KEY_S;
    keyCodes["D"] = GLFW_KEY_D;

    // Add more key codes as needed
    keyCodes["UP"] = GLFW_KEY_UP;
    keyCodes["DOWN"] = GLFW_KEY_DOWN;
    keyCodes["LEFT"] = GLFW_KEY_LEFT;
    keyCodes["RIGHT"] = GLFW_KEY_RIGHT;
    keyCodes["ENTER"] = GLFW_KEY_ENTER;

    // Mouse button constants
    auto mouseButtons = m_LuaState->create_named_table("MouseButton");
    mouseButtons["LEFT"] = GLFW_MOUSE_BUTTON_LEFT;
    mouseButtons["RIGHT"] = GLFW_MOUSE_BUTTON_RIGHT;
    mouseButtons["MIDDLE"] = GLFW_MOUSE_BUTTON_MIDDLE;
}

/**
 * @brief Registers 2D rendering functions in the Lua environment
 *
 * @param engine The engine table to add the functions to
 */
void LuaScriptSystem::Register2DRendererAPI(sol::table& engine) {
    // Begin a new 2D rendering scene
    engine.set_function("renderer2DBeginScene", []() -> bool {
        auto& renderer = Engine::Renderer2D::Get();
        auto camera = std::make_shared<Engine::OrthographicCamera>(
            DEFAULT_ORTHO_LEFT, DEFAULT_ORTHO_RIGHT, DEFAULT_ORTHO_BOTTOM, DEFAULT_ORTHO_TOP);
        renderer.BeginScene(camera);
        return true;
    });

    // End the current 2D rendering scene
    engine.set_function("renderer2DEndScene", []() -> bool {
        Engine::Renderer2D::Get().EndScene();
        return true;
    });

    // Draw a colored quad
    engine.set_function("drawQuad",
                        [](float x, float y, float width, float height, float r, float g, float b,
                           float a) -> bool {
                            if (width <= 0 || height <= 0) {
                                LOG_WARN("Quad dimensions should be positive");
                                return false;
                            }

                            if (r < 0.0f || r > 1.0f || g < 0.0f || g > 1.0f || b < 0.0f ||
                                b > 1.0f || a < 0.0f || a > 1.0f) {
                                LOG_WARN("Color values should be between 0.0 and 1.0");
                            }

                            Engine::Renderer2D::Get().DrawQuad({x, y}, {width, height},
                                                               {r, g, b, a});
                            return true;
                        });

    // Draw a textured quad
    engine.set_function(
        "drawTexturedQuad",
        [](float x, float y, float width, float height,
           const std::shared_ptr<Engine::Texture>& texture, float tilingFactor) -> bool {
            if (width <= 0 || height <= 0) {
                LOG_WARN("Quad dimensions should be positive");
                return false;
            }

            if (!texture) {
                LOG_ERROR_CONCAT("Texture cannot be null");
                return false;
            }

            Engine::Renderer2D::Get().DrawQuad({x, y}, {width, height}, texture, tilingFactor);
            return true;
        });

    // Create a checker texture
    engine.set_function("createCheckerTexture", []() -> std::shared_ptr<Engine::Texture> {
        uint8_t data[CHECKER_TEXTURE_SIZE * CHECKER_TEXTURE_SIZE * 4] = {0};
        for (int y = 0; y < CHECKER_TEXTURE_SIZE; y++) {
            for (int x = 0; x < CHECKER_TEXTURE_SIZE; x++) {
                int index = (y * CHECKER_TEXTURE_SIZE + x) * 4;
                if ((x + y) % 2 == 0) {
                    data[index + 0] = 255;
                    data[index + 1] = 255;
                    data[index + 2] = 255;
                    data[index + 3] = 255;
                } else {
                    data[index + 0] = 0;
                    data[index + 1] = 0;
                    data[index + 2] = 0;
                    data[index + 3] = 255;
                }
            }
        }
        auto texture =
            std::make_shared<Engine::Texture>(CHECKER_TEXTURE_SIZE, CHECKER_TEXTURE_SIZE);
        texture->SetData(data, sizeof(data));
        return texture;
    });

    // Add renderer initialization function
    engine.set_function("renderer2DInitialize", []() -> bool {
        Engine::Renderer2D::Get().Initialize();
        return true;
    });
}

/**
 * @brief Registers object creation and management functions in the Lua environment
 *
 * @param engine The engine table to add the functions to
 */
void LuaScriptSystem::RegisterObjectManagementAPI(sol::table& engine) {
    // Create a new cube object in the active scene
    engine.set_function("createCube",
                        [this](const std::string& name) -> std::shared_ptr<SceneObject> {
                            if (name.empty()) {
                                LOG_ERROR_CONCAT("Object name cannot be empty");
                                return nullptr;
                            }

                            return this->WithActiveScene(
                                [&name](auto scene) -> std::shared_ptr<SceneObject> {
                                    auto cube = scene->CreateObject(name);
                                    if (!cube) {
                                        LOG_ERROR_CONCAT("Failed to create object: ", name);
                                        return nullptr;
                                    }

                                    cube->SetMesh(AssetManager::Get().GetOrCreateCubeMesh());
                                    auto shader = ShaderLibrary::CreateBasicShader();
                                    if (shader) {
                                        auto material = std::make_shared<Material>(shader);
                                        material->SetVector4("u_Color", glm::vec4(1.0f));
                                        cube->SetMaterial(material);
                                    }
                                    LOG_INFO_CONCAT("Created cube object: ", name);
                                    return cube;
                                },
                                "No active scene to create cube in");
                        });

    // Get an existing object by name from the active scene
    engine.set_function("getObject",
                        [this](const std::string& name) -> std::shared_ptr<SceneObject> {
                            if (name.empty()) {
                                LOG_ERROR_CONCAT("Object name cannot be empty");
                                return nullptr;
                            }

                            return this->WithActiveScene(
                                [&name](auto scene) -> std::shared_ptr<SceneObject> {
                                    auto object = scene->GetObject(name);
                                    if (!object) {
                                        LOG_WARN("Object not found in scene: ", name);
                                        return nullptr;
                                    }
                                    return std::dynamic_pointer_cast<SceneObject>(object);
                                },
                                "No active scene when trying to get object: " + name);
                        });
}

/**
 * @brief Executes a Lua script from a provided script string with timeout protection
 *
 * This method attempts to load and execute a Lua script directly from a string.
 * It performs two-stage validation: first checking script loading, then script execution.
 * The script execution is protected by a timeout to prevent infinite loops.
 *
 * @param script A string containing the Lua script to be executed
 * @param timeoutMs Maximum execution time in milliseconds (0 for no timeout)
 * @return bool True if the script is successfully loaded and executed, false otherwise
 */
bool LuaScriptSystem::ExecuteScriptWithTimeout(const std::string& script, uint32_t timeoutMs) {
    sol::load_result loadResult = m_LuaState->load(script);
    if (!loadResult.valid()) {
        sol::error err = loadResult;
        LOG_ERROR_CONCAT("Failed to load script: ", err.what());
        return false;
    }

    // Create protected function for execution
    sol::protected_function protected_fn = loadResult;

    bool success = true;
    try {
        // Start execution timer
        auto start = std::chrono::high_resolution_clock::now();

        // Execute the script
        sol::protected_function_result result = protected_fn();

        // Check execution time if timeout is set
        if (timeoutMs > 0) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration =
                std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            if (duration > timeoutMs) {
                LOG_ERROR_CONCAT("Script execution exceeded timeout (", timeoutMs, "ms)");
                success = false;
            }
        }

        if (success && !result.valid()) {
            sol::error err = result;
            LOG_ERROR_CONCAT("Failed to execute script: ", err.what());
            success = false;
        }
    } catch (const std::exception& e) {
        LOG_ERROR_CONCAT("Script execution error: ", e.what());
        success = false;
    }

    return success;
}

/**
 * @brief Executes a Lua script from a provided script string.
 *
 * This method attempts to load and execute a Lua script directly from a string.
 * It performs two-stage validation: first checking script loading, then script execution.
 *
 * @param script A string containing the Lua script to be executed
 * @return bool True if the script is successfully loaded and executed, false otherwise
 *
 * @note Logs detailed error messages if script loading or execution fails
 * @note Uses sol2 library for Lua script handling
 *
 * @exception None Exceptions are caught and logged internally
 *
 * @example
 * LuaScriptSystem luaSystem;
 * bool success = luaSystem.ExecuteScript("print('Hello, World!')");
 * // success will be true if script executes without errors
 */
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

/**
 * @brief Attempts to locate and execute a Lua script file from multiple potential search paths.
 *
 * This method searches for the specified script file in several predefined locations:
 * 1. The original path provided
 * 2. Relative paths with "../" and "../../" prefixes
 * 3. Predefined script directories with the filename extracted from the original path
 *
 * @param originalPath The initial file path of the Lua script to be executed
 * @return bool True if the script is successfully found and executed, false otherwise
 *
 * @details
 * - Searches through a list of potential file paths
 * - Uses FileSystem::Exists() to verify file presence
 * - Logs an error if the script cannot be found
 * - Attempts to load and execute the script using sol2 library
 * - Logs an error if script loading or execution fails
 *
 * @note Supports relative and absolute file paths
 * @note Extracts filename from the original path for additional search locations
 */
bool LuaScriptSystem::ExecuteFile(const std::string& originalPath) {
    std::vector<std::string> searchPaths = {
        originalPath, "../" + originalPath, "../../" + originalPath,
        "../sandbox/assets/scripts/" + originalPath.substr(originalPath.find_last_of("/\\") + 1),
        "sandbox/assets/scripts/" + originalPath.substr(originalPath.find_last_of("/\\") + 1),
        "assets/scripts/" + originalPath.substr(originalPath.find_last_of("/\\") + 1),
    };

    std::string validPath;
    bool found = false;

    for (const auto& path : searchPaths) {
        if (FileSystem::Exists(path)) {
            validPath = path;
            LOG_TRACE_CONCAT("Script found: ", validPath);
            found = true;
            break;
        }
    }

    if (!found) {
        LOG_ERROR_CONCAT("Script not found: ", originalPath);
        LOG_TRACE_CONCAT("Current directory: ", std::filesystem::current_path());
        return false;
    }

    sol::load_result loadResult = m_LuaState->load_file(validPath);
    if (!loadResult.valid() || !loadResult().valid()) {
        LOG_ERROR_CONCAT("Failed to execute script: ", validPath);
        return false;
    }

    // Add to loaded scripts and set up file watching
    m_LoadedScripts.insert(validPath);
    m_ScriptWatcher.WatchFile(
        validPath, [this, validPath](const std::string&) { this->ReloadFile(validPath); });

    return true;
}

bool LuaScriptSystem::ReloadFile(const std::string& filepath) {
    if (!FileSystem::Exists(filepath)) {
        LOG_ERROR_CONCAT("Cannot reload missing script: ", filepath);
        return false;
    }

    // Store previous global state
    sol::table oldGlobals = (*m_LuaState)["_G"];
    sol::table oldEngine = (*m_LuaState)["engine"];

    // Create a new temporary state for validation
    sol::state tempState;
    // Execute in main state
    tempState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string,
                             sol::lib::table, sol::lib::io, sol::lib::os);

    // Try loading in temporary state first
    sol::load_result loadResult = tempState.load_file(filepath);
    if (!loadResult.valid()) {
        sol::error err = loadResult;
        LOG_ERROR_CONCAT("Failed to load updated script: ", filepath, " - ", err.what());
        return false;
    }

    // Execute in temporary state to check for syntax errors
    sol::protected_function_result result = loadResult();
    if (!result.valid()) {
        sol::error err = result;
        LOG_ERROR_CONCAT("Failed to execute updated script: ", filepath, " - ", err.what());
        return false;
    }

    // Before loading into main state, preserve old globals
    std::unordered_map<std::string, sol::object> preservedGlobals;
    for (const auto& pair : oldGlobals) {
        if (pair.first.is<std::string>()) {
            std::string key = pair.first.as<std::string>();
            // Only preserve Lua-defined globals (skip C++ objects)
            if (pair.second.get_type() == sol::type::table ||
                pair.second.get_type() == sol::type::function ||
                pair.second.get_type() == sol::type::string ||
                pair.second.get_type() == sol::type::number ||
                pair.second.get_type() == sol::type::boolean) {
                preservedGlobals[key] = pair.second;
            }
        }
    }

    // Load into main state
    sol::load_result mainLoadResult = m_LuaState->load_file(filepath);
    if (!mainLoadResult.valid()) {
        sol::error err = mainLoadResult;
        LOG_ERROR_CONCAT("Failed to reload script in main state: ", filepath, " - ", err.what());
        return false;
    }

    // Execute in main state
    sol::protected_function_result mainResult = mainLoadResult();
    if (!mainResult.valid()) {
        sol::error err = mainResult;
        LOG_ERROR_CONCAT("Failed to execute reloaded script in main state: ", filepath, " - ",
                         err.what());
        return false;
    }

    // Restore preserved globals
    for (const auto& [key, value] : preservedGlobals) {
        (*m_LuaState)["_G"][key] = value;
    }

    LOG_INFO_CONCAT("Successfully reloaded script: ", filepath);

    // Notify callback if registered
    if (m_ReloadCallback) {
        m_ReloadCallback(filepath);
    }

    return true;
}

void LuaScriptSystem::CallGlobalFunction(const std::string& functionName) {
    sol::protected_function fn = (*m_LuaState)[functionName];
    if (!fn.valid()) {
        return;
    }

    sol::protected_function_result result = fn();
    if (!result.valid()) {
        LOG_WARN("Failed to call global function: ", functionName);
    }
}

/**
 * @brief Creates a sandboxed environment for script execution
 *
 * This method creates a restricted execution environment that limits
 * access to potentially dangerous libraries and functions.
 *
 * @param allowedLibs Map of libraries to allow and whether they should be restricted
 * @return sol::environment The sandboxed environment
 */
sol::environment LuaScriptSystem::CreateSandbox(
    const std::unordered_map<std::string, bool>& allowedLibs) {
    sol::environment env(*m_LuaState, sol::create, m_LuaState->globals());

    // Remove potentially dangerous libraries by default
    env["os"] = sol::nil;
    env["io"] = sol::nil;
    env["package"] = sol::nil;
    env["require"] = sol::nil;

    // Add allowed libraries back, possibly with restrictions
    for (const auto& [lib, unrestricted] : allowedLibs) {
        if (lib == "os" && !unrestricted) {
            // Create restricted os table
            sol::table safe_os = m_LuaState->create_table();

            // Copy safe functions directly
            if ((*m_LuaState)["os"]["time"].valid()) safe_os["time"] = (*m_LuaState)["os"]["time"];

            if ((*m_LuaState)["os"]["date"].valid()) safe_os["date"] = (*m_LuaState)["os"]["date"];

            if ((*m_LuaState)["os"]["clock"].valid())
                safe_os["clock"] = (*m_LuaState)["os"]["clock"];

            // Exclude dangerous functions like execute, remove, etc.
            env["os"] = safe_os;
        } else if (lib == "io" && !unrestricted) {
            // Create restricted io table (read-only)
            sol::table safe_io = m_LuaState->create_table();

            // Copy read function directly
            if ((*m_LuaState)["io"]["read"].valid()) safe_io["read"] = (*m_LuaState)["io"]["read"];

            // Exclude write operations
            env["io"] = safe_io;
        } else if (allowedLibs.find(lib) != allowedLibs.end()) {
            // Add the entire library if it's allowed and unrestricted
            env[lib] = (*m_LuaState)[lib];
        }
    }

    // Always make the engine API available
    env["engine"] = (*m_LuaState)["engine"];

    return env;
}

/**
 * @brief Execute a Lua script in a sandboxed environment with limited capabilities
 *
 * @param script The Lua script to execute
 * @param allowedLibs Map of libraries to allow and whether they should be unrestricted
 * @return bool True if execution succeeded, false otherwise
 */
bool LuaScriptSystem::ExecuteScriptSandboxed(
    const std::string& script, const std::unordered_map<std::string, bool>& allowedLibs) {
    // Create the sandbox environment
    sol::environment env = CreateSandbox(allowedLibs);

    // Instead of using set_environment, we'll use Lua's run_script with the environment
    try {
        // Wrap the script with "return function() ... end" to create a function
        // that will run in the environment
        std::string wrapped_script = "return function() " + script + " end";

        // Load the wrapped script
        sol::function setup = m_LuaState->script(wrapped_script);
        if (!setup.valid()) {
            LOG_ERROR_CONCAT("Failed to load script for sandbox");
            return false;
        }

        // Get the function to run in the environment
        sol::function target = setup();

        // Run the function with the environment
        sol::protected_function_result result = target(env);

        if (!result.valid()) {
            sol::error err = result;
            LOG_ERROR_CONCAT("Failed to execute sandboxed script: ", err.what());
            return false;
        }
    } catch (const std::exception& e) {
        LOG_ERROR_CONCAT("Error in sandboxed execution: ", e.what());
        return false;
    }

    return true;
}

}  // namespace Engine
