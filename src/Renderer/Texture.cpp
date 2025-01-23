#include "Texture.h"
#include "stb_image.h"

namespace Engine {
    /** @brief Creates a texture from an image file
     *  @param path Path to the image file
     *  @return Shared pointer to the created texture
     */
    std::shared_ptr<Texture> Texture::Create(const std::string& path) {
        return std::shared_ptr<Texture>(new Texture(path));
    }

    /** @brief Creates an empty texture with specified dimensions
     *  @param width Width of the texture in pixels
     *  @param height Height of the texture in pixels
     *  @return Shared pointer to the created texture
     */
    std::shared_ptr<Texture> Texture::Create(uint32_t width, uint32_t height) {
        return std::shared_ptr<Texture>(new Texture(width, height));
    }

    /** @brief Constructor that loads a texture from a file
     *  @param path Path to the image file
     */
    Texture::Texture(const std::string& path) 
        : m_Path(path) {
        stbi_set_flip_vertically_on_load(1);
        
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4) {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3) {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /** @brief Constructor that creates an empty texture
     *  @param width Width of the texture in pixels
     *  @param height Height of the texture in pixels
     */
    Texture::Texture(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height) {
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /** @brief Destructor that cleans up OpenGL resources */
    Texture::~Texture() {
        glDeleteTextures(1, &m_RendererID);
    }

    /** @brief Binds the texture to a specific texture slot
     *  @param slot The texture unit slot to bind to
     */
    void Texture::Bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    /** @brief Unbinds the texture */
    void Texture::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /** @brief Default constructor */
    Texture::Texture()
        : m_RendererID(0), m_Width(0), m_Height(0), m_BPP(0) {
        m_Type = ResourceType::Texture;
    }

    /** @brief Loads a texture from a file
     *  @param path Path to the image file
     *  @return true if loading succeeded, false otherwise
     */
    bool Texture::Load(const std::string& path) {
        m_Path = path;
        stbi_set_flip_vertically_on_load(1);
        
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
        if (!data) {
            return false;
        }
        
        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4) {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3) {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        m_IsLoaded = true;
        
        return true;
    }

    /** @brief Unloads the texture and frees GPU resources */
    void Texture::Unload() {
        if (m_RendererID) {
            glDeleteTextures(1, &m_RendererID);
            m_RendererID = 0;
        }
        m_IsLoaded = false;
    }
}
