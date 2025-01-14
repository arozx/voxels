#pragma once

#include <pch.h>

namespace Engine {

/**
 * @brief Enumerates different types of game resources
 */
enum class ResourceType {
    Texture,    ///< 2D or 3D texture resource
    Shader,     ///< Shader program resource
    Mesh,       ///< 3D mesh resource
    Sound,      ///< Audio resource
    Model       ///< 3D model resource
};

/**
 * @brief Base class for all loadable game resources
 * 
 * Provides reference counting and basic resource management functionality
 * that all resource types must implement.
 */
class Resource {
public:
    Resource() = default;
    virtual ~Resource() = default;
    
    /**
     * @brief Loads the resource from the specified path
     * @param path File path to the resource
     * @return true if loading was successful
     */
    virtual bool Load(const std::string& path) = 0;

    /**
     * @brief Unloads the resource from memory
     */
    virtual void Unload() = 0;
    
    /** @return The file path of the resource */
    const std::string& GetPath() const { return m_Path; }
    /** @return The type of the resource */
    ResourceType GetType() const { return m_Type; }
    /** @return Whether the resource is currently loaded */
    bool IsLoaded() const { return m_IsLoaded; }
    
    /** @brief Increments the reference count */
    void AddRef() { m_RefCount++; }
    /** @brief Decrements the reference count and unloads if zero */
    void Release() { 
        m_RefCount--; 
        if (m_RefCount <= 0) {
            Unload();
        }
    }
    /** @return Current reference count */
    int GetRefCount() const { return m_RefCount; }

protected:
    std::string m_Path;       ///< File path of the resource
    ResourceType m_Type;      ///< Type of the resource
    bool m_IsLoaded = false;  ///< Whether the resource is currently loaded
    int m_RefCount = 0;       ///< Reference count of the resource
};

}