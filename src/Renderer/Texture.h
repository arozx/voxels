#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <stb_image.h>

#include "../Core/Resource.h"

namespace Engine {

/**
 * @class Texture
 * @brief Represents a 2D texture resource that can be loaded from file or created empty
 * @extends Resource
 */
class Texture : public Resource {
public:
    /**
     * @brief Default constructor
     */
    Texture();

    /**
     * @brief Constructs a texture from an image file
     * @param path Path to the image file
     */
    Texture(const std::string& path);

    /**
     * @brief Constructs an empty texture with specified dimensions
     * @param width Width of the texture in pixels
     * @param height Height of the texture in pixels
     */
    Texture(uint32_t width, uint32_t height);

    /**
     * @brief Destructor
     */
    virtual ~Texture();

    /**
     * @brief Loads texture data from a file
     * @param path Path to the image file
     * @return true if loading succeeded, false otherwise
     */
    virtual bool Load(const std::string& path) override;

    /**
     * @brief Unloads the texture data from GPU memory
     */
    virtual void Unload() override;

    /**
     * @brief Creates a new texture from a file
     * @param path Path to the image file
     * @return Shared pointer to the new texture
     */
    static std::shared_ptr<Texture> Create(const std::string& path);

    /**
     * @brief Creates a new empty texture
     * @param width Width of the texture in pixels
     * @param height Height of the texture in pixels
     * @return Shared pointer to the new texture
     */
    static std::shared_ptr<Texture> Create(uint32_t width, uint32_t height);
    
    /**
     * @brief Binds the texture to a specific texture unit
     * @param slot Texture unit to bind to (default: 0)
     */
    void Bind(uint32_t slot = 0) const;

    /**
     * @brief Unbinds the texture
     */
    void Unbind() const;
    
    /**
     * @brief Gets the width of the texture
     * @return Width in pixels
     */
    uint32_t GetWidth() const { return m_Width; }

    /**
     * @brief Gets the height of the texture
     * @return Height in pixels
     */
    uint32_t GetHeight() const { return m_Height; }

    /**
     * @brief Gets the OpenGL renderer ID
     * @return OpenGL texture handle
     */
    uint32_t GetRendererID() const { return m_RendererID; }

    /**
     * @brief Sets raw texture data
     * @param data Pointer to pixel data (must be RGBA format)
     * @param size Size of data in bytes (must be width * height * 4)
     */
    virtual void SetData(void* data, uint32_t size) {
        ASSERT(size == m_Width * m_Height * 4 && "Data must be entire texture in RGBA format!");
        Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     data);
    }

private:
    uint32_t m_RendererID = 0;          ///< OpenGL texture handle
    int m_Width = 0, m_Height = 0;      ///< Texture dimensions
    int m_BPP = 0;                      ///< Bits per pixel
    unsigned char* m_LocalBuffer = nullptr; ///< Local pixel data buffer
    std::string m_Path;                 ///< Path to texture file
};

}
