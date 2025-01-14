#pragma once
#include "../Core/Resource.h"
#include <stb_image.h>
#include <glad/glad.h>

namespace Engine {

class Texture : public Resource {
public:
    Texture();
    Texture(const std::string& path);
    Texture(uint32_t width, uint32_t height);
    virtual ~Texture();

    virtual bool Load(const std::string& path) override;
    virtual void Unload() override;

    static std::shared_ptr<Texture> Create(const std::string& path);
    static std::shared_ptr<Texture> Create(uint32_t width, uint32_t height);
    
    void Bind(uint32_t slot = 0) const;
    void Unbind() const;
    
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
    uint32_t GetRendererID() const { return m_RendererID; }

private:
    uint32_t m_RendererID = 0;
    int m_Width = 0, m_Height = 0;
    int m_BPP = 0;
    unsigned char* m_LocalBuffer = nullptr;
    std::string m_Path;
};

}
