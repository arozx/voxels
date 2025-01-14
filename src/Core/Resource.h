#pragma once

#include <pch.h>

namespace Engine {

enum class ResourceType {
    Texture,
    Shader,
    Mesh,
    Sound,
    Model
};

class Resource {
public:
    Resource() = default;
    virtual ~Resource() = default;
    
    virtual bool Load(const std::string& path) = 0;
    virtual void Unload() = 0;
    
    const std::string& GetPath() const { return m_Path; }
    ResourceType GetType() const { return m_Type; }
    bool IsLoaded() const { return m_IsLoaded; }
    
    void AddRef() { m_RefCount++; }
    void Release() { 
        m_RefCount--; 
        if (m_RefCount <= 0) {
            Unload();
        }
    }
    int GetRefCount() const { return m_RefCount; }

protected:
    std::string m_Path;
    ResourceType m_Type;
    bool m_IsLoaded = false;
    int m_RefCount = 0;
};

}