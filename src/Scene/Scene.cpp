#include "Scene.h"

#include <pch.h>

#include "TerrainSystem/TerrainSystem.h"

namespace Engine {
/**
 * @brief Constructs a SceneObject with the specified name.
 * 
 * @param name A string representing the name of the scene object.
 * 
 * @details Initializes a SceneObject by setting its name. This constructor creates
 * a basic scene object without any additional setup of transform, children, or parent.
 * 
 * @note The object is created with a default state and can be further configured 
 * using other methods like AddChild() or setting transforms.
 */

/**
 * @brief Adds a child SceneObject to the current object's hierarchy.
 *
 * This method establishes a parent-child relationship between the current SceneObject
 * and the provided child object. The child's parent is set to the current object,
 * and the child is added to the current object's list of children.
 *
 * @param child A shared pointer to the SceneObject to be added as a child.
 * @note The method uses shared_from_this() to create a shared pointer reference to the current
 * object.
 */

/**
 * @brief Computes the world transformation matrix for the scene object.
 *
 * Calculates the object's world transformation by combining its local transformation
 * with its parent's world transformation. If the object has no parent, returns its
 * local transformation matrix.
 *
 * @return glm::mat4 The complete world transformation matrix for the scene object.
 * @note The transformation is computed hierarchically, multiplying the parent's world
 *       transform with the object's local transform to preserve scene graph hierarchy.
 */

/**
 * @brief Constructs a new Scene with the specified name.
 *
 * Initializes the scene by creating a root SceneObject, setting up a TerrainSystem,
 * and logging the scene creation. The root object serves as the top-level container
 * for all scene objects in the hierarchy.
 *
 * @param name A string representing the name of the scene.
 *
 * @note Creates a default root SceneObject named "Root"
 * @note Initializes a new TerrainSystem for the scene
 * @note Logs a trace message with the scene name upon creation
 */

Scene::Scene(const std::string &name) : m_Name(name) {
    m_RootObject = std::make_shared<SceneObject>("Root");
    LOG_TRACE("Created scene: ", m_Name);  // Fix: Use _CONCAT for string concatenation
}

/**
 * @brief Creates a new scene object and adds it to the scene.
 *
 * This method instantiates a new SceneObject with the specified name, adds it to
 * the scene's managed objects list, and optionally sets it as the root object if
 * no root has been established.
 *
 * @param name The name to assign to the newly created scene object.
 * @return std::shared_ptr<SceneObject> A shared pointer to the newly created scene object.
 *
 * @note If no root object exists when this method is called, the created object
 * becomes the scene's root object.
 *
 * @see SceneObject
 * @see m_Objects
 * @see m_RootObject
 */
std::shared_ptr<SceneObject> Scene::CreateObject(const std::string &name) {
    // Create a new scene object with the given name
    auto object = std::make_shared<SceneObject>(name);

    // Add to managed objects container
    m_Objects.push_back(object);

    // If there's no root object, set this as the root
    if (!m_RootObject) {
        m_RootObject = object;
    }

    LOG_TRACE("Created scene object: ", name);
    return object;
}

/**
 * @brief Destructor for the Scene class.
 *
 * Cleans up resources associated with the scene, specifically resetting the terrain system.
 * This ensures proper resource management and prevents potential memory leaks.
 *
 * @note Resets the terrain system smart pointer, which will automatically 
 * release any resources held by the terrain system.
 */
Scene::~Scene() {
    // Ensure that all resources are cleaned up
    m_TerrainSystem.reset();
}

/**
 * @brief Initializes the scene, creating a TerrainSystem if enabled.
 *
 * Logs initialization. If terrain is enabled and no TerrainSystem exists,
 * creates a new one and initializes it with the renderer.
 */
void Scene::OnCreate() {
    LOG_TRACE("Initializing scene: ", m_Name);

    // Only create terrain if it was explicitly enabled and requested
    if (m_EnableTerrain && !m_TerrainSystem) {
        CreateTerrain();
    }

    // Initialize existing terrain if we have one and a renderer
    if (m_TerrainSystem && m_Renderer) {
        m_TerrainSystem->Initialize(*m_Renderer);
    }
}

/**
 * @brief Placeholder method for scene activation.
 *
 * This method is intended to be overridden by derived scene classes to provide
 * custom activation logic. In the base implementation, it does nothing.
 *
 * @note Derived classes can implement specific initialization or setup tasks
 *       that should occur when the scene becomes active.
 */
void Scene::OnActivate() {
    // Empty implementation - can be overridden by derived classes
}

/**
 * @brief Deactivates the scene.
 *
 * This method serves as a placeholder for scene deactivation logic in derived classes.
 * It provides a default empty implementation that can be overridden to perform
 * specific cleanup or state management when a scene is deactivated.
 *
 * @note Intended to be implemented by child classes to define custom deactivation behavior.
 */
void Scene::OnDeactivate() {
    // Empty implementation - can be overridden by derived classes
}

/**
 * @brief Updates the terrain system with the given delta time.
 *
 * This method checks if a terrain system is present and calls its update method.
 * If no terrain system is initialized, no action is taken.
 *
 * @param deltaTime The time elapsed since the last update, used for time-dependent terrain modifications.
 */
void Scene::OnUpdate(float deltaTime) {
    if (m_TerrainSystem) {
        m_TerrainSystem->Update(deltaTime);
    }
}

/**
 * @brief Renders the scene by rendering terrain and scene objects.
 *
 * This method manages the rendering process for the scene. It first ensures the renderer
 * is initialized by storing its reference and calling OnCreate if not already done.
 * Then it renders the terrain system (if present) followed by rendering the root scene object
 * and its entire hierarchy.
 *
 * @param renderer Reference to the rendering system used to draw scene elements.
 *
 * @note If no renderer was previously set, this method will trigger scene initialization.
 * @note Terrain is rendered before scene objects to establish background rendering.
 */
void Scene::OnRender(Renderer &renderer) {
    // Store renderer reference and initialize if needed
    if (!m_Renderer) {
        m_Renderer = &renderer;
        OnCreate();
    }

    // Render terrain first
    if (m_TerrainSystem) {
        m_TerrainSystem->Render(renderer);
    }

    // Then render scene objects
    if (m_RootObject) {
        RenderObject(m_RootObject, renderer);
    }
}

/**
 * @brief Recursively renders a scene object and its children.
 *
 * This method traverses the scene graph, rendering each object with a valid vertex array and material.
 * It prepares a render command with the object's world transformation matrix and submits it to the renderer.
 * The method then recursively renders all child objects of the current object.
 *
 * @param object A shared pointer to the SceneObject to be rendered.
 * @param renderer The Renderer instance used to submit rendering commands.
 *
 * @note If the object is null or lacks a vertex array or material, it will be skipped.
 * @note The rendering is performed using the object's world transformation matrix.
 */
void Scene::RenderObject(const std::shared_ptr<SceneObject> &object, Renderer &renderer) {
    if (object->GetMesh() && object->GetMaterial()) {
        RenderCommand cmd;
        cmd.vertexArray = object->GetMesh();
        cmd.material = object->GetMaterial();
        cmd.transformMatrix = object->GetWorldTransform();
        cmd.primitiveType = GL_TRIANGLES;
        renderer.Submit(cmd.vertexArray, cmd.material, cmd.transformMatrix);
    }

    for (const auto &child : object->children) {
        if (child) {
            RenderObject(child, renderer);
        }
    }
}

bool Scene::CreateTerrain() {
    if (!m_TerrainSystem) {
        m_TerrainSystem = std::make_unique<TerrainSystem>();
        m_EnableTerrain = true;
        if (m_Renderer) {
            m_TerrainSystem->Initialize(*m_Renderer);
        }
    }
    return m_TerrainSystem != nullptr;
}
}  // namespace Engine