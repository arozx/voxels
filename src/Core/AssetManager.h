#pragma once

#include <pch.h>
#include <unordered_set>
#include "Resource.h"

namespace Engine {

class AssetManager {
public:
    static AssetManager& Get() {
        static AssetManager instance;
        return instance;
    }

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
            LOG_ERROR("Failed to load resource: {}", path);
            return nullptr;
        }

        resource->AddRef();
        m_ResourceCache[path] = resource;
        return resource;
    }

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

    bool IsFrequentlyUsed(const std::string& path) const {
        return m_FrequentlyUsedPaths.find(path) != m_FrequentlyUsedPaths.end();
    }

    void PreloadFrequentAssets() {
        // Example paths that could be frequently used
        std::vector<std::string> frequentTextures = {
            "assets/textures/ui/common.png",
            "assets/textures/terrain/blocks.png",
            "assets/textures/particles/effects.png"
        };

        for (const auto& path : frequentTextures) {
            auto texture = LoadResource<Texture>(path);
            if (texture) {
                MarkAsFrequentlyUsed<Texture>(path);
            }
        }
    }

    void UnloadResource(const std::string& path) {
        auto it = m_ResourceCache.find(path);
        if (it != m_ResourceCache.end()) {
            it->second->Release();
            if (it->second->GetRefCount() <= 0) {
                m_ResourceCache.erase(it);
            }
        }
    }

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

    template<typename T>
    std::future<std::shared_ptr<T>> LoadResourceAsync(const std::string& path) {
        return std::async(std::launch::async, [this, path]() {
            return LoadResource<T>(path);
        });
    }

    // Track memory usage
    size_t GetTotalMemoryUsage() const { return m_TotalMemoryUsage; }
    void UpdateMemoryUsage(size_t delta) { m_TotalMemoryUsage += delta; }

private:
    AssetManager() = default;
    ~AssetManager() { UnloadAll(); }

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

    std::unordered_map<std::string, std::shared_ptr<Resource>> m_ResourceCache;
    std::unordered_map<std::string, std::shared_ptr<Resource>> m_FrequentResourceCache;
    std::unordered_set<std::string> m_FrequentlyUsedPaths;
    size_t m_TotalMemoryUsage = 0;
};

}