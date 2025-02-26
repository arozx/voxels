#pragma once

#include <pch.h>

#include <unordered_set>

#include "../Renderer/Buffer.h"
#include "../Renderer/Texture.h"
#include "../Renderer/VertexArray.h"
#include "Resource.h"

namespace Engine {

/**
 * @brief Manages loading, caching, and unloading of game assets
 * @details Implements a singleton pattern for global asset management with support
 *          for frequently used resources and asynchronous loading
 */
class AssetManager {
public:
    /**
     * @brief Get the singleton instance of the AssetManager
     * @return Reference to the AssetManager instance
     */
    static AssetManager& Get() {
        static AssetManager instance;
        return instance;
    }

    /**
     * @brief Load a resource of specified type from path
     * @tparam T The resource type to load
     * @param path Path to the resource file
     * @return Shared pointer to the loaded resource
     */
    template<typename T>
    std::shared_ptr<T> LoadResource(const std::string& path) {
        // Check cache first
        auto it = m_ResourceCache.find(path);
        if (it != m_ResourceCache.end()) {
            auto resource = std::dynamic_pointer_cast<T>(it->second);
            if (resource) {
                resource->AddRef();
                return resource;
            }
        }

        // Create new resource
        auto resource = std::make_shared<T>();
        if (!resource->Load(path)) {
            LOG_ERROR("Failed to load resource: ", path);
            return nullptr;
        }

        resource->AddRef();
        m_ResourceCache[path] = resource;
        return resource;
    }

    /**
     * @brief Mark a resource as frequently used to prevent automatic unloading
     * @tparam T The resource type
     * @param path Path to the resource
     */
    template<typename T>
    void MarkAsFrequentlyUsed(const std::string& path) {
        auto it = m_ResourceCache.find(path);
        if (it != m_ResourceCache.end()) {
            m_FrequentlyUsedPaths.insert(path);
            if (auto resource = std::dynamic_pointer_cast<T>(it->second)) {
                m_FrequentResourceCache[path] = resource;
            }
        }
    }

    /**
     * @brief Remove a resource from the frequently used list
     * @param path Path to the resource
     */
    void UnmarkAsFrequentlyUsed(const std::string& path) {
        m_FrequentlyUsedPaths.erase(path);
        // Move back to regular cache if still in use
        auto it = m_FrequentResourceCache.find(path);
        if (it != m_FrequentResourceCache.end()) {
            if (it->second->GetRefCount() > 0) {
                m_ResourceCache[path] = it->second;
            }
            m_FrequentResourceCache.erase(it);
        }
    }

    /**
     * @brief Check if a resource is marked as frequently used
     * @param path Path to the resource
     * @return true if the resource is frequently used
     */
    bool IsFrequentlyUsed(const std::string& path) const {
        return m_FrequentlyUsedPaths.find(path) != m_FrequentlyUsedPaths.end();
    }

    /** @brief Preload frequently used assets into memory */
    void PreloadFrequentAssets() {
        // Texture preloading moved to TextureManager
    }

    /**
     * @brief Decrease reference count of a resource and potentially unload it
     * @param path Path to the resource
     */
    void UnloadResource(const std::string& path) {
        auto it = m_ResourceCache.find(path);
        if (it != m_ResourceCache.end()) {
            it->second->Release();
            if (it->second->GetRefCount() <= 0) {
                m_ResourceCache.erase(it);
            }
        }
    }

    /** @brief Unload all resources that are not marked as frequently used and have no references */
    void UnloadUnused() {
        // Only remove non-frequent resources that have no references
        for (auto it = m_ResourceCache.begin(); it != m_ResourceCache.end();) {
            if (it->second->GetRefCount() <= 0 && !IsFrequentlyUsed(it->first)) {
                it = m_ResourceCache.erase(it);
            } else {
                ++it;
            }
        }
    }

    /**
     * @brief Asynchronously load a resource
     * @tparam T The resource type to load
     * @param path Path to the resource
     * @return Future containing the loaded resource
     */
    template<typename T>
    std::future<std::shared_ptr<T>> LoadResourceAsync(const std::string& path) {
        return std::async(std::launch::async, [this, path]() {
            return LoadResource<T>(path);
        });
    }

    /**
     * @brief Get total memory usage of loaded resources
     * @return Size in bytes of memory used
     */
    size_t GetTotalMemoryUsage() const { return m_TotalMemoryUsage; }

    /**
     * @brief Update the total memory usage counter
     * @param delta Change in memory usage
     */
    void UpdateMemoryUsage(size_t delta) { m_TotalMemoryUsage += delta; }

    /**
     * @brief Get or create a cube mesh
     * @return Shared pointer to the cube mesh
     */
    std::shared_ptr<VertexArray> GetOrCreateCubeMesh() {
        static std::shared_ptr<VertexArray> cubeMesh;
        if (!cubeMesh) {
            float vertices[] = {
                // Front face
                -0.5f,
                -0.5f,
                0.5f,
                0.5f,
                -0.5f,
                0.5f,
                0.5f,
                0.5f,
                0.5f,
                -0.5f,
                0.5f,
                0.5f,
                // Back face
                -0.5f,
                -0.5f,
                -0.5f,
                0.5f,
                -0.5f,
                -0.5f,
                0.5f,
                0.5f,
                -0.5f,
                -0.5f,
                0.5f,
                -0.5f,
            };

            uint32_t indices[] = {
                0, 1, 2, 2, 3, 0,  // Front
                1, 5, 6, 6, 2, 1,  // Right
                5, 4, 7, 7, 6, 5,  // Back
                4, 0, 3, 3, 7, 4,  // Left
                3, 2, 6, 6, 7, 3,  // Top
                4, 5, 1, 1, 0, 4   // Bottom
            };

            cubeMesh = std::shared_ptr<VertexArray>(VertexArray::Create());
            auto vb =
                std::shared_ptr<VertexBuffer>(VertexBuffer::Create(vertices, sizeof(vertices)));
            auto ib = std::shared_ptr<IndexBuffer>(
                IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

            BufferLayout layout = {{ShaderDataType::Float3, "aPosition"}};

            vb->SetLayout(layout);
            cubeMesh->AddVertexBuffer(vb);
            cubeMesh->SetIndexBuffer(ib);
        }
        return cubeMesh;
    }

private:
    AssetManager() = default;
    ~AssetManager() { UnloadAll(); }

    /**
     * @brief Unload all resources from memory
     */
    void UnloadAll() {
        // Unload regular cache
        for (auto& [path, resource] : m_ResourceCache) {
            if (!IsFrequentlyUsed(path)) {
                resource->Unload();
            }
        }
        
        // Unload frequent cache
        for (auto& [path, resource] : m_FrequentResourceCache) {
            resource->Unload();
        }
        
        m_ResourceCache.clear();
        m_FrequentResourceCache.clear();
        m_FrequentlyUsedPaths.clear();
        m_TotalMemoryUsage = 0;
    }

    std::unordered_map<std::string, std::shared_ptr<Resource>> m_ResourceCache; ///< Main resource cache
    std::unordered_map<std::string, std::shared_ptr<Resource>> m_FrequentResourceCache; ///< Cache for frequently used resources
    std::unordered_set<std::string> m_FrequentlyUsedPaths; ///< Set of paths marked as frequently used
    size_t m_TotalMemoryUsage = 0; ///< Total memory used by all resources
};

}