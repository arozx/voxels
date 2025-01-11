#pragma once
#include "../pch.h"
#include <stb_image.h>
#include <glad/glad.h>

namespace Engine {
    class Texture {
    public:
        static std::shared_ptr<Texture> Create(const std::string& path);
        static std::shared_ptr<Texture> Create(uint32_t width, uint32_t height);
        
        virtual ~Texture();
        
        void Bind(uint32_t slot = 0) const;
        void Unbind() const;
        
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetTextureID() const { return m_TextureID; }

    private:
        Texture(const std::string& path);
        Texture(uint32_t width, uint32_t height);
        
        uint32_t m_TextureID;
        uint32_t m_Width, m_Height;
        std::string m_Path;
    };
}
